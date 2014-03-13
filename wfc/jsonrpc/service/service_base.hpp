#pragma once
#include <iostream>

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
  ::wfc::io::callback writer;
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

  std::shared_ptr<handler_base> _handler_prototype;
  
  service_base(io_service_type& io_service, const options_type& opt, const handler_base& handler )
    : super( io_service, opt)
    //, handler(std::bind( &self::operator(), this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) )
    //, tmp_worker(io_service, opt)
  {
    super::create(*this);
    _handler_prototype = handler.clone();
  }
  
  std::function<void(incoming_holder holder)> _tmp_response_handler;

  // 
  template<typename T>
  void process_result( T& , incoming_holder holder, ::wfc::io::callback)
  {
    _tmp_response_handler( std::move(holder) );
  }
  
  
  
  void send_request(
    ::wfc::io::io_id_t io_id,
    const char* name,
    handler_base::incoming_handler_t response_handler,
    handler_base::request_serializer_t serializer
  )
  {
    this->dispatch([this, io_id, name, response_handler, serializer]()
    {
      if ( auto wrk = this->get_worker(name) )
      {
        auto itr = _io_map.find(io_id);
        if (itr!=_io_map.end())
        {
          int id = 333;
          auto writer = itr->second.writer;
          _tmp_response_handler = response_handler;
          wrk->dispatch([writer, name, id, serializer](){
            auto d = serializer(name, id);
            writer( std::move(d) );
          });
        }
      }
      /*
      auto itr = _io_map.find(io_id);
      if ( itr != _io_map.end() )
      {
        itr->second
      }
      */
    });
    /*
    _tmp_response_handler = response_handler;
    auto data = serializer(333);
    */
  };
  
  /*
  typedef std::function< ::wfc::io::data_ptr(int id) > request_serializer_t;
  typedef std::function<void(incoming_holder holder)> incoming_handler_t;
  typedef std::function< void(incoming_handler_t, request_serializer_t) > outgoing_request_handler_t;
  
  outgoing_request_handler_t outgoing_request_handler;
  */

  
  /*
  typedef std::function< void(io_id_t, callback, add_shutdown_handler )> startup_handler_t;
  */
  
  // TODO: сделать связку с method-handler
  // TODO: убрать
  /*
  typedef std::map< ::wfc::io::io_id_t, ::wfc::io::callback> io_map_t;
  io_map_t _io_map;
  */
  
  /*
  std::weak_ptr<handler_base> get_handler(::wfc::io::io_id_t io_id)
  {
    auto itr = _io_map.find( io_id );
    if ( itr != _io_map.end() )
    {
      return itr->second.method_handler; 
    }
    return std::weak_ptr<handler_base>;
  }
  */

  // Для тестирования (и клиента)
  // !!! до запуска
  void attach_handler(::wfc::io::io_id_t io_id, std::shared_ptr<handler_base> handler, ::wfc::io::callback writer)
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
  
  
  // По сути реестр connections
  typedef std::map< ::wfc::io::io_id_t, io_data > io_map;
  io_map _io_map;
  
  
  
  // Новый коннект
  void startup_handler(::wfc::io::io_id_t io_id, ::wfc::io::callback writer, ::wfc::io::add_shutdown_handler add_shutdown )
  {
    
    if ( writer == nullptr)
    {
      // Костыль (из конфига прут аккцепторы)
      return;
    }
    
    this->post([this, io_id, writer/*, add_shutdown*/]()
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
      }
      else
      {
        abort();
      }
      
      
    });
    
    add_shutdown( this->strand().wrap( [this](::wfc::io::io_id_t io_id)
    {
      std::cout << "jsonrpc shutdown " << io_id << std::endl;
      _io_map.erase(io_id);
    }));
 }
  

  //typedef std::function<void(data_ptr, io_id_t, callback )> handler;
  /// Для входящих запросов
  void operator()( data_ptr d, ::wfc::io::io_id_t id, ::wfc::io::callback callback)
  {
    typedef std::chrono::high_resolution_clock clock_t;
    clock_t::time_point now = clock_t::now();
    
    ::wfc::io::callback tp_callback = [now, callback]( ::wfc::io::data_ptr d)
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
  template<typename PReq, typename Serializer, typename Callback>
  void send_request3( const char* name, PReq , Serializer , Callback  )
  {
    
  }

  
  typedef std::shared_ptr< worker_type > worker_ptr;
  typedef std::list<worker_ptr> worker_list;
  typedef std::pair<worker_list, typename worker_list::iterator > pair_worker_list;
  typedef std::map< std::string, pair_worker_list> method_map;
  typedef std::list< std::thread > thread_list;
  typedef std::shared_ptr< ::wfc::io_service> io_service_ptr;
  typedef std::list<io_service_ptr> service_list; 
  
  method_map _method_map;
  worker_list _workers;
  service_list _services;
  thread_list _threads;
  
  
  
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

  template<typename T>
  void push_advice(T& , incoming_holder holder, std::weak_ptr<handler_base> hb, ::wfc::io::callback callback)
  {
    if ( holder.has_method() )
    {
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
    }
  } 

  
  template<typename T>
  void start_advice(T& t)
  {
    auto services = t.options().services;
    if (services.empty())
      services.push_back( options::service());
    
    for (auto &s : services)
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
      
      for (auto w: s.queues)
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
    // TODO: dispatch
    super::stop(*this);
  }

  //::wfc::io::handler handler;
  
  //std::shared_ptr<worker_type> tmp_worker;
};

}} // wfc


