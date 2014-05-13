#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <wfc/jsonrpc/service/service_aspect.hpp>

#include <wfc/io/io_base.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace jsonrpc{

/*
jsonrpc:[
  // Один io_service
  {
    threads: 4, 
    queues: [
      { 
        count:3,
        methods:["*"]
      }
      
    ]
  }
]

std::shared_ptr< worker > worker_ptr; 
std::list< worker_ptr > worker_list; 
std::pair< worker_list, worker_list::iterator > list_pair;
std::map< std::string, list_pair>
std::list< io_service_ptr > _service;
std::list< threads > _threads;
worker_list _workers;
*/

struct io_data
{
  std::shared_ptr<handler_base> method_handler;
  ::wfc::io::outgoing_handler_t writer;
  
  io_data( std::shared_ptr<handler_base> method_handler, ::wfc::io::outgoing_handler_t writer)
    : method_handler(method_handler)
    , writer(writer)
  {}
  
  //typedef std::function<void(incoming_holder holder)> response_handler;
  typedef handler_base::incoming_handler_t response_handler;
  typedef std::map<int, response_handler> response_map;
  response_map response;
};
/*

struct io_data_by_id
{
  bool operator()(const io_data& left, const io_data& right)
  {
    return left.id < right.id;
  }
};
*/

  
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
 
  // По сути реестр connections
  typedef std::map< ::wfc::io::io_id_t, io_data > io_map;
  typedef std::map< int, ::wfc::io::io_id_t> call_io_map;

  
  
  service_base(io_service_type& io_service, const options_type& opt, const handler_base& handler )
    : super( io_service, opt)
    , _call_id_counter(0)
  {
    super::create(*this);
    _handler_prototype = handler.clone();
  }
  
  //std::function<void(incoming_holder holder)> _tmp_response_handler;

  handler_base::incoming_handler_t handler_by_call_id(int call_id) const
  {
    auto itr = this->_call_io_map.find(call_id);
    if ( itr!=this->_call_io_map.end() )
    {
      auto itr2 = this->_io_map.find(itr->second);
      if ( itr2!=this->_io_map.end() )
      {
        auto itr3 = itr2->second.response.find(call_id);
        if ( itr3 != itr2->second.response.end() )
        {
          return itr3->second;
        }
        else
        {
          DAEMON_LOG_ERROR("jsonrpc::service: jsonrpc id=" << call_id << " not found in response list");
        }
      }
      else
      {
        COMMON_LOG_WARNING("jsonrpc::service: io id=" << itr->second << " not found");
      }
    }
    else
    {
      COMMON_LOG_WARNING("jsonrpc::service: jsonrpc id=" << call_id << " not found");
    }
    return nullptr;
  }
  
  // 
  template<typename T>
  void process_result( T& , incoming_holder holder, ::wfc::io::outgoing_handler_t)
  {
    
    if ( auto handler = this->handler_by_call_id( holder.get_id<int>() ) )
    {
      handler( std::move(holder) );
    }
    
    /*
    int call_id = holder.get_id<int>();
    auto itr = this->_call_io_map.find(call_id);
    if ( itr!=this->_call_io_map.end() )
    {
      auto itr2 = this->_io_map.find(itr->second);
      if ( itr2!=this->_io_map.end() )
      {
        auto itr3 = itr2->second.response.find(call_id);
        if ( itr3 != itr2->second.response.end() )
        {
          itr3->second( std::move(holder) );
        }
      }
      else
      {
        COMMON_LOG_WARNING("jsonrpc::service: io id=" << itr->second << " not found");
      }
    }
    else
    {
      COMMON_LOG_WARNING("jsonrpc::service: jsonrpc id=" << call_id << " not found");
    }
    */
  }
  
  
  void send_notify(
    ::wfc::io::io_id_t io_id,
    const char* name,
    handler_base::notify_serializer_t serializer
  )
  {
    this->dispatch([this, io_id, name, serializer]()
    {
      if ( auto wrk = this->get_worker(name) )
      {
        auto itr = _io_map.find(io_id);
        if (itr!=_io_map.end())
        {
          auto writer = itr->second.writer;
          wrk->dispatch([writer, name, serializer](){
            auto d = serializer(name);
            writer( std::move(d) );
          });
        }
      }
    });
  };
  
  
  void send_request(
    ::wfc::io::io_id_t io_id,
    const char* name,
    handler_base::incoming_handler_t result_handler,
    handler_base::request_serializer_t serializer
  )
  {
    this->dispatch([this, io_id, name, result_handler, serializer]()
    {
      if ( auto wrk = this->get_worker(name) )
      {
        
        auto itr = this->_io_map.find(io_id);
        if (itr!=this->_io_map.end())
        {
          int id = ++this->_call_id_counter;
          auto writer = itr->second.writer;
          itr->second.response.insert( std::make_pair(id, result_handler));
          
          this->_call_io_map[id] = io_id;
          wrk->dispatch([writer, name, id, serializer]()
          {
            auto d = serializer(name, id);
            writer( std::move(d) );
          });
        }
      }
    });
  };
  
  

  // Для тестирования (и клиента)
  // !!! до запуска
  /*
  void attach_handler(::wfc::io::io_id_t io_id, std::shared_ptr<handler_base> handler, ::wfc::io::outgoing_handler_t writer)
  {
    //this->dispatch([this, io_id, writer, handler]()
    {
        handler->outgoing_request_handler = [io_id, this](const char* name, handler_base::incoming_handler_t result_hander, handler_base::request_serializer_t serializer)
        {
          this->send_request( io_id, name, result_hander, serializer);
        };
      
        // TODO: проверка на существование id
        _io_map.insert( 
          std::make_pair(
            io_id, 
            io_data(
            { 
              handler, 
              writer 
            })
          )  
        );
    }//);
  }
  */
  
  
  
  
  
  // Новый коннект
  void startup_handler(::wfc::io::io_id_t io_id, ::wfc::io::outgoing_handler_t writer, ::wfc::io::add_shutdown_handler_t add_shutdown )
  {
    
    if ( writer == nullptr)
    {
      
      // Костыль (из конфига прут аккцепторы)
      return;
    }
    
    this->post([this, io_id, writer]()
    {
      
      auto itr = _io_map.find(io_id);
      if ( itr == _io_map.end() )
      {
        auto handler = _handler_prototype->clone();
        
        //typedef std::function< void(incoming_handler_t, request_serializer_t) > outgoing_request_handler_t;
        
        handler->outgoing_request_handler = [io_id, this](const char* name, handler_base::incoming_handler_t result_hander, handler_base::request_serializer_t serializer)
        {
          this->send_request( io_id, name, result_hander, serializer);
        };
        
        handler->outgoing_notify_handler = [io_id, this](const char* name, handler_base::notify_serializer_t serializer)
        {
          this->send_notify( io_id, name, serializer);
        };
        
        this->_io_map.insert( 
          std::make_pair(
            io_id, 
            io_data(handler, writer)
          )  
        );
        
        handler->start(io_id);

      }
      else
      {
        abort();
      }
      
      
    });
    
    add_shutdown( this->strand().wrap( [this](::wfc::io::io_id_t io_id)
    {
      // TODO: Сейчас dispatch, сделать post, иначе может выполниться раньше поста выше
      auto itr = _io_map.find(io_id);
      if ( itr != _io_map.end() )
      {
        itr->second.method_handler->stop(io_id);
        for ( auto& r : itr->second.response )
        {
          this->_call_io_map.erase(r.first);
        }
        this->_io_map.erase(itr);
      }
    }));
 }
  

  //typedef std::function<void(data_ptr, io_id_t, callback )> handler;
  /// Для входящих запросов
  void operator()( data_ptr d, ::wfc::io::io_id_t id, ::wfc::io::outgoing_handler_t callback)
  {
    
    typedef std::chrono::high_resolution_clock clock_t;
    clock_t::time_point now = clock_t::now();
    
    ::wfc::io::outgoing_handler_t tp_callback = [now, callback]( ::wfc::io::data_ptr d)
    {
      callback( std::move(d) );
    };
    
    
    auto dd = std::make_shared<data_ptr>( std::move(d) );
    this->post([this, dd, /*iio*/id, tp_callback]()
    {
      this->get_aspect().template get<_incoming_>()( *this, std::move(*dd), /*iio*/id, std::move(tp_callback) );
    });
  }
  
  // Для исходящих запросов
  /*
  template<typename PReq, typename Serializer, typename Callback>
  void send_request3( const char* name, PReq , Serializer , Callback  )
  {
    
  }
  */

  
  

  
  
  worker_ptr get_worker(const std::string& name)
  {
    return this->get_worker(name.c_str());
  }
  
  worker_ptr get_worker(const char* name)
  {
    auto itr = _method_map.find( name );
    if ( itr == _method_map.end() )
    {
      itr = _method_map.find( "*" );
    }

    if ( itr!=_method_map.end() )
    {
      if ( itr->second.first.end() == itr->second.second )
      {
        itr->second.second = itr->second.first.begin();
      }
      return *(itr->second.second++);
    }
    return nullptr;
  }
  
  /*
  template<typename T>
  void push_advice(T& , incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::outgoing_handler_t callback)
  {
    if ( holder.has_method() )
    {
      if ( auto wrk = this->get_worker( holder.method() ) )
      {
        wrk->operator()( std::move(holder), hb, callback );
      }
      else
      {
        COMMON_LOG_WARNING("jsonrpc method not allowed: " << holder.method() )
      }
    }*/
      /*
      auto itr = _method_map.find( holder.method() );
      
      if ( itr == _method_map.end() )
      {
        itr = _method_map.find( "*" );
      }
      
      if ( itr!=_method_map.end() )
      {
        if ( itr->second.first.end() == itr->second.second )
        {
          itr->second.second = itr->second.first.begin();
        }
        // Отдаем воркеру
        (*(itr->second.second))->operator()( std::move(holder), hb, callback );
        ++(itr->second.second);
      }
      else
      {
        COMMON_LOG_WARNING("jsonrpc method not allowed: " << holder.method() )
      }
      
    }*/
  /*}*/ 

  
  template<typename T>
  void start_advice(T& t)
  {
    auto workers = t.options().workers;
    if (workers.empty())
      workers.push_back( options::worker());
    
    for (auto &s : workers)
    {
      // Ахтунг! вынести в мембер
      io_service_ptr io_ptr = nullptr;
      if (s.threads != 0)
      {
        io_ptr = std::make_shared< ::wfc::io_service>();
        _services.push_back(io_ptr);
      }
      
      ::wfc::io_service& io_ref = (io_ptr == nullptr ? t.get_io_service() : *io_ptr );
      //wfc::io_service* io_ref = &(io_ptr == nullptr ? t.get_io_service() : *io_ptr );
      
      /*
      wfc::io_service* io_raw = nullptr;
      if ( io_ptr == nullptr )
        io_raw = &(t.get_io_service());
      else
        io_raw = io_ptr.get();
      */
      
      for (auto w: s.strands)
      {
        options_type opt = t.options();
        
        for (int i = 0; i < w.count; ++i)
        {
          auto wrk = std::make_shared<worker_type>( io_ref, t.options() );
          _workers.push_back( wrk );
          for (auto& s : w.methods)
          {
            auto itr = _method_map.find(s);
            if ( itr == _method_map.end() )
            {
              pair_worker_list pwl;
              itr = _method_map.insert( std::make_pair(s, pwl) ).first;
            }
            itr->second.first.push_back(wrk);
            if ( itr->second.first.size() == 1)
              itr->second.second = itr->second.first.begin();
          }
        }
      }
      
      for (int i=0; i < s.threads; ++i)
      {
        _threads.push_back( std::thread([io_ptr]() {
          ::wfc::io_service::work wrk(*io_ptr);
          io_ptr->run();
        }));
      }
    }
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
    /*this->dispatch([this]()
    {
      this->start_no_tf();
      //tmp_worker = std::make_shared<worker_type>(this->get_io_service(), this->options() );
    });
    */
  }
  
  void stop()
  {
    DEBUG_LOG_BEGIN("jsonrpc::stop()")
    
    /*
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;
    */

    // TODO: dispatch
    //super::stop(*this);
    //this->dispatch([this, &mtx, &cv, &ready]()
    {
      DEBUG_LOG_BEGIN("jsonrpc::stop() dispatch")
      for (auto& s: this->_services)
        s->stop();
      
      for (auto& t: this->_threads)
        t.join();
      
      this->_workers.clear();
      this->_threads.clear();
      this->_method_map.clear();
      this->_services.clear();
      this->_io_map.clear();
      this->_call_io_map.clear();
      
      DEBUG_LOG_END("jsonrpc::stop() dispatch")
    }/*);*/
    

    DEBUG_LOG_END("jsonrpc::stop()")
  }

public:
  
  int _call_id_counter;
  std::shared_ptr<handler_base> _handler_prototype;

  
  // Эту великолепную четверку в worker manager
  method_map _method_map;
  worker_list _workers;
  service_list _services;
  thread_list _threads;
  
  // Эту в outgoing manager (менеджер исходящих запросов)
  io_map _io_map;
  call_io_map _call_io_map;
  
};

}} // wfc


