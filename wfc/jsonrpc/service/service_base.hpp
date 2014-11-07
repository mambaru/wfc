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


template<
  typename A = fas::aspect<>, 
  template<typename> class AspectClass = fas::aspect_class 
>
class service_base
  : public io::basic_io<A, AspectClass>
  , public std::enable_shared_from_this< service_base<A, AspectClass> >
{
public:
  typedef ihandler handler_interface; // TODO: В io_registry
  typedef service_base<A, AspectClass> self;
  typedef io::basic_io<A, AspectClass> super;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::aspect::template advice_cast<_worker_type_>::type worker_type;
  
  typedef handler_interface::io_id_t io_id_t;
  typedef handler_interface::outgoing_handler_t outgoing_handler_t;
  typedef handler_interface::result_handler_t result_handler_t;
  typedef handler_interface::notify_serializer_t notify_serializer_t;
  typedef handler_interface::request_serializer_t request_serializer_t;

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
  
  service_base(
    io_service_type& io_service, 
    const options_type& opt, 
    const handler_interface& handler 
  ) 
    : super( io_service, opt)
    , _worker_manager( io_service, opt)
    , _active(false)
  {
    super::create(*this);
    _handler_prototype = handler.clone();
  }
  
  
  std::shared_ptr< worker_type > get_worker(const std::string& name)
  {
    return this->get_worker(name.c_str());
  }
  
  std::shared_ptr< worker_type > get_worker(const char* name)
  {
    return _worker_manager.get_worker(name);
  }

  const io_registry& registry() const
  {
    return this->_io_reg;
  }

  io_registry& registry()
  {
    return this->_io_reg;
  }

  void send_notify(
    io_id_t io_id,
    const char* name,
    notify_serializer_t serializer
  )
  {
    if (_active)
    {
      if ( auto writer = this->registry().get_outgoing_handler( io_id ) )
      {
        if ( auto wrk = this->get_worker(name) )
        {
          wrk->post([writer, name, serializer](){
            auto d = serializer(name);
            writer( std::move(d) );
          });
        }
        else if ( this->options().workers.empty() )
        {
          auto d = serializer(name);
          writer( std::move(d) );
        }
        else
        {
          COMMON_LOG_WARNING("jsonrpc worker unavailable for method: " << name )
        }
      }
      else
      {
        COMMON_LOG_WARNING("Requester not found for io_id=" << io_id << " for method: " << name )
      }
    }
  };
  
  int tmp = 0;
  void send_request(
    io_id_t io_id,
    const char* name,
    result_handler_t result_handler,
    request_serializer_t serializer
  )
  {
    ++tmp;
#warning DEBUG
    if ( tmp < 1000000 )
      DAEMON_LOG_MESSAGE("DEBUG -1-: service_base::send_request N" << tmp);
    
    if (_active)
    {
      auto requester = this->registry().add_result_handler( io_id, result_handler );
      if ( requester.second != nullptr )
      {
        if ( auto wrk = this->get_worker(name) )
        {
          if ( tmp < 1000000 ) 
            DAEMON_LOG_MESSAGE("DEBUG -2.1-: service_base::send_request N" << tmp);
          wrk->post([this, io_id, requester, name,  serializer]()
          {
            if ( this->tmp < 1000000 ) 
              DAEMON_LOG_MESSAGE("DEBUG -2.2-: service_base::send_request N" << this->tmp);
            auto d = serializer(name, requester.first);
            requester.second( std::move(d) );
          });
        }
        else if ( this->options().workers.empty() )
        {
          if ( tmp < 1000000 ) DAEMON_LOG_MESSAGE("DEBUG -3-: service_base::send_request N" << tmp);
          auto d = serializer(name, requester.first);
          requester.second( std::move(d) );
        }
        else
        {
          COMMON_LOG_WARNING("jsonrpc worker unavailable for method: " << name )
        }
      }
      else
      {
        COMMON_LOG_WARNING("Requester not found for io_id=" << io_id << " for method: " << name )
      }
    }
  };
  
  // Новый коннект
  void create_handler( io_id_t io_id, outgoing_handler_t outgoing_handler, ::wfc::io::add_shutdown_handler_t add_shutdown /*TODO: в typedef*/ )
  {
    if ( outgoing_handler != nullptr && add_shutdown!=nullptr)
    {
      super::get_aspect().template get<_add_shutdown_>()(*this, std::move(add_shutdown) );
      super::get_aspect().template get<_create_handler_>()(*this, io_id, std::move(outgoing_handler) );      
    }
    else
    {
      DAEMON_LOG_ERROR("jsonrpc::service::create_handler: invalid args ")
      abort();
    }
 }

  /// Для входящих запросов
  void operator()( data_ptr d, io_id_t io_id, outgoing_handler_t outgoing_handler)
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
    this->start_no_tf();
    //this->dispatch( std::bind( &self::start_no_tf, this) );
    _active = true;
  }
  
  void stop()
  {
    _active = false;
    // TODO: dispatch
    _worker_manager.stop();
  }
  
  std::shared_ptr<handler_interface> get_prototype() const
  {
    return _handler_prototype;
  }
  
  std::shared_ptr<handler_interface> clone_prototype() const
  {
    if ( _handler_prototype != nullptr )
    {
      return _handler_prototype->clone();
    }
    else
    {
      return nullptr;
    }
  }

private:
  std::shared_ptr<handler_interface> _handler_prototype;
  worker_manager _worker_manager; // TODO: в аспект
  io_registry _io_reg; // TODO: в аспект
  bool _active;
};

}} // wfc

