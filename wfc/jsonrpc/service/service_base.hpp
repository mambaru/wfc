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

  service_base(io_service_type& io_service, const options_type& opt)
    : super( io_service, opt)
    , handler(std::bind( &self::operator(), this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) )
    //, tmp_worker(io_service, opt)
  {
    super::create(*this);
  }

  void operator()( data_ptr d, std::weak_ptr<wfc::io::iio> iio, wfc::io::callback&& handler)
  {
    auto dd = std::make_shared<data_ptr>( std::move(d) );
    this->dispatch([this, dd, iio, handler]()
    {
      std::cout << "jsonrpc_base" << std::endl; 
      this->get_aspect().template get<_incoming_>()( *this, std::move(*dd), iio, std::move(handler) );
    });
  }
  
  typedef std::shared_ptr< worker_type > worker_ptr;
  typedef std::list<worker_ptr> worker_list;
  typedef std::pair<worker_list, typename worker_list::iterator > pair_worker_list;
  typedef std::map< std::string, pair_worker_list> method_map;
  typedef std::list< std::thread > thread_list;
  typedef std::shared_ptr< wfc::io_service> io_service_ptr;
  typedef std::list<io_service_ptr> service_list; 
  
  method_map _method_map;
  worker_list _workers;
  service_list _services;
  thread_list _threads;
  
  template<typename T>
  void start_advice(T& t)
  {
    auto services = t.options().services;
    if (services.empty())
      services.push_back( service_options::service());
    
    for (auto &s : services)
    {
      io_service_ptr io_ptr = nullptr;
      if (s.threads != 0)
        io_ptr = std::make_shared<wfc::io_service>();
      
      wfc::io_service& io_ref = (io_ptr == nullptr ? t.get_io_service() : *io_ptr );
      for (auto w: s.queues)
      {
        options_type opt = t.options();
        wfc::io_service ios;
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
      
      for (int i=0; i < s.threads; ++i)
      {
        _threads.push_back( std::thread([&io_ref]() {
          io_ref.run();
        }));
      }
    }
  }
  
  void start()
  {
    // TODO: dispatch
    super::start(*this);
    start_advice(*this);
    tmp_worker = std::make_shared<worker_type>(this->get_io_service(), this->options() );
  }
  
  void stop()
  {
    // TODO: dispatch
    super::stop(*this);
  }

  wfc::io::handler handler;
  
  std::shared_ptr<worker_type> tmp_worker;
};

}} // wfc


