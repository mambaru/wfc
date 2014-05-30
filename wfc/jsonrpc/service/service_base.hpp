#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <wfc/jsonrpc/service/service_aspect.hpp>
#include <wfc/jsonrpc/service/io_registry.hpp>
#include <wfc/jsonrpc/service/worker_manager.hpp>

#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{


template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class service_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< service_base<A, AspectClass> >
{
public:
  typedef service_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::aspect::template advice_cast<_worker_type_>::type worker_type;

  /// ////////////////////////////
  
  typedef std::shared_ptr< worker_type > worker_ptr;
  typedef std::list<worker_ptr> worker_list;
  typedef std::pair<worker_list, typename worker_list::iterator > pair_worker_list;
  typedef std::map< std::string, pair_worker_list> method_map;
  typedef std::list< std::thread > thread_list;
  typedef std::shared_ptr< ::wfc::io_service> io_service_ptr;
  typedef std::list<io_service_ptr> service_list; 
 
  virtual ~service_base()
  {
    this->stop();
  }
  
  service_base(io_service_type& io_service, const options_type& opt, const handler_base& handler )
    : super( io_service, opt)
    , _worker_manager( io_service, opt)
  {
    super::create(*this);
    _handler_prototype = handler.clone();
  }
  
  
  std::weak_ptr< worker_type > get_worker(const std::string& name)
  {
    return this->get_worker(name.c_str());
  }
  
  std::weak_ptr< worker_type > get_worker(const char* name)
  {
    return _worker_manager.get_worker(name);
  }

  const io_registry& registry() const
  {
    return this->_io_reg;
  }
  /*
  handler_base::result_handler_t get_result_handler(int call_id) const
  {
    return this->_io_reg.get_result_handler(call_id);
  }
  
  std::weak_ptr<handler_base> get_jsonrpc_handler(io::io_id_t io_id) const 
  {
    return this->_io_reg.get_jsonrpc_handler(io_id);
  }
  */
  
  // 
  /*
  template<typename T>
  void process_result( T& , incoming_holder holder, ::wfc::io::outgoing_handler_t)
  {
    if ( auto handler = this->registry().get_result_handler( holder.get_id<int>() ) )
    {
      handler( std::move(holder) );
    }
  }
  */
  
  void send_notify(
    ::wfc::io::io_id_t io_id,
    const char* name,
    handler_base::notify_serializer_t serializer
  )
  {
    this->dispatch([this, io_id, name, serializer]()
    {
      if ( auto wrk = this->get_worker(name).lock() )
      {
        auto writer = this->_io_reg.get_outgoing_handler( io_id );
        wrk->dispatch([writer, name, serializer](){
          auto d = serializer(name);
          writer( std::move(d) );
        });
      }
    });
  };
  
  
  void send_request(
    ::wfc::io::io_id_t io_id,
    const char* name,
    handler_base::result_handler_t result_handler,
    handler_base::request_serializer_t serializer
  )
  {
    this->dispatch([this, io_id, name, result_handler, serializer]()
    {
      if ( auto wrk = this->get_worker(name).lock() )
      {
        auto requester = this->_io_reg.add_result_handler( io_id, result_handler );
        
        wrk->post([requester, name,  serializer]()
        {
          auto d = serializer(name, requester.first);
          requester.second( std::move(d) );
        });
      }
    });
  };
  
  
  // Новый коннект
  void startup_handler( io::io_id_t io_id, io::outgoing_handler_t outgoing_handler, io::add_shutdown_handler_t add_shutdown )
  {
    if ( outgoing_handler == nullptr)
    {
      // Костыль (из конфига прут аккцепторы)
      return;
    }
    
    this->post([this, io_id, outgoing_handler]()
    {
      
      auto handler = _handler_prototype->clone();
     
      handler->send_request = [io_id, this](const char* name, handler_base::result_handler_t result_hander, handler_base::request_serializer_t serializer)
      {
        this->send_request( io_id, name, result_hander, serializer);
      };
        
      handler->send_notify = [io_id, this](const char* name, handler_base::notify_serializer_t serializer)
      {
        this->send_notify( io_id, name, serializer);
      };
        
      this->_io_reg.set_io(io_id, handler, outgoing_handler);
      
      handler->start(io_id);
    });
    
    add_shutdown( this->strand().wrap( [this](::wfc::io::io_id_t io_id)
    {
      // TODO: Сейчас dispatch, сделать post, иначе может выполниться раньше поста выше
      if ( auto handler = this->_io_reg.erase_io(io_id) )
      {
        handler->stop(io_id);
      }
    }));
 }
  

  //typedef std::function<void(data_ptr, io_id_t, callback )> handler;
  /// Для входящих запросов
  void operator()( data_ptr d, io::io_id_t io_id, ::wfc::io::outgoing_handler_t outgoing_handler)
  {
    this->get_aspect().template get<_input_>()( *this, std::move(d), io_id, std::move(outgoing_handler) );
  }
  
  
  
  template<typename T>
  void start_advice(T& )
  {
    _worker_manager.start();
  }
  
  std::vector<std::string> get_methods() const
  {
    return _handler_prototype->get_methods();
  }
  
  void start_no_tf()
  {
    super::start(*this);
    start_advice(*this);
  }
  
  void start()
  {
    this->dispatch( std::bind( &self::start_no_tf, this) );
  }
  
  void stop()
  {
    // TODO: dispatch
    _worker_manager.stop();
  }
  
  std::weak_ptr<handler_base> get_prototype() const
  {
    return _handler_prototype;
  }
  
  std::shared_ptr<handler_base> clone_prototype() const
  {
    return _handler_prototype->clone();
  }

private:
  std::shared_ptr<handler_base> _handler_prototype;
  worker_manager _worker_manager;
  io_registry _io_reg;
};

}} // wfc


