#pragma once

#include <wfc/io_service.hpp>
#include <wfc/io/server/tags.hpp>
#include <wfc/io/basic_io.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/memory.hpp>

#include <list>
#include <thread>
#include <memory>

namespace wfc{ namespace io{ namespace server{
 
struct _acceptor_manager_;
struct _configure_server_;
struct _start_server_;
struct _io_services_;
  
struct ad_configure
{
  template<typename T, typename Conf>
  void operator()(T& t, const Conf& conf)
  {
    typedef typename T::aspect::template advice_cast<_acceptor_type_>::type acceptor_type;
    
    auto& acceptors = t.get_aspect().template get<_acceptors_>();
    while ( acceptors.size() > static_cast<size_t>(conf.threads) )
    {
      acceptors.pop_back();
    }
    
    typedef typename acceptor_type::descriptor_type descriptor_type;
    descriptor_type desc( t.get_io_service() );
    auto acceptor_proto = std::make_shared<acceptor_type>( std::move(desc), conf);
    
    COMMON_LOG_MESSAGE("listen " << t.options().host << ":" << t.options().port)
    acceptor_proto->listen();
    
    size_t threads = static_cast<size_t>(t.options().threads);
    
    if ( threads == 0)
    {
      acceptors.push_back( acceptor_proto );
      return;
    }

    auto& services = t.get_aspect().template get<_io_services_>();
    
    auto itr = acceptors.begin();
    for (size_t i = 0 ; i < static_cast<size_t>(t.options().threads); ++i, ++itr)
    {
      if ( acceptors.size() < threads )
      {
        auto io = std::make_shared< ::wfc::io_service>();
        services.push_back(io);
        acceptors.push_back( acceptor_proto->clone(*io));
      }
      else
      {
        (*itr)->reconfigure(conf);
      }
    }
  }
};

struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    auto& acceptors = t.get_aspect().template get<_acceptors_>();
    
    for(auto& ptr : acceptors)
    {
      ptr->start();
    }

    auto& threads   = t.get_aspect().template get<_threads_>();
    threads.clear();
    
    auto& services = t.get_aspect().template get<_io_services_>();
    for (auto s : services)
    {
      threads.push_back( std::make_unique<std::thread>( [&t, s]()
      { 
        s->run();
      }));
    }
  }  
};

struct _before_stop_;

struct ad_before_stop
{
  template<typename T>
  void operator()(T& t)
  {
    auto& acceptors = t.get_aspect().template get<_acceptors_>();
    
    for (auto& a : acceptors)
    {
      // сначала закрываем, чтоб реконнект на другой ассептор не прошел 
      a->close();
    }

    for (auto& a : acceptors)
    {
      // stop - снихронная операция
      a->stop(nullptr);
    }
    auto& services = t.get_aspect().template get<_io_services_>();
    for (auto& s : services)
    {
      s->stop();
    }
    
    auto& threads   = t.get_aspect().template get<_threads_>();
    for (auto& t : threads)
    {
      t->join();
    }
    acceptors.clear();
    threads.clear();
    services.clear();
  }
};


struct server_options
{
  int threads = 0;
};

template<typename AcceptorOptions>
struct options
  : AcceptorOptions
  , server_options
{
};


template<typename Acceptor, typename ServerOptions>
class acceptor_manager
{
  typedef Acceptor acceptor_type;
  
  typedef std::shared_ptr<acceptor_type>       acceptor_ptr;
  typedef std::shared_ptr<std::thread>         thread_ptr;
  typedef ::wfc::io_service io_service_type;
  typedef std::shared_ptr<io_service_type>  io_service_ptr;
  
  typedef std::list<acceptor_ptr>    acceptor_list;
  typedef std::list<thread_ptr>      thread_list;
  typedef std::list<io_service_ptr>  io_service_list;
  typedef std::mutex mutex_type;
  
public:
  
  typedef ServerOptions options_type;
  
  /*typedef AcceptorOptions acceptor_options_type;
  typedef server_options<acceptor_options_type> options_type;*/
  
  
  acceptor_manager(io_service_type& io_service, const options_type& opt/*, acceptor_ptr acceptor_proto*/)
    : _io_service(io_service)
    , _opt(opt)
    //, _acceptor_proto(acceptor_proto)
  {}
  
  void reconfigure(const options_type& opt)
  {
    // std::lock_guard<mutex_type> lk(_mutex);
    _opt = opt;
    this->_reconfigure_and_start();
  }
  
  void start()
  {
    // std::lock_guard<mutex_type> lk(_mutex);
    this->_reconfigure_and_start();
  }
  
  void stop()
  {
    DEBUG_LOG_BEGIN("acceptor stop...")
    /*
    if ( _acceptors.empty()) 
    {
      _acceptor_proto->close();
      _acceptor_proto->stop(nullptr);
      return;
    }*/
    
    // std::lock_guard<mutex_type> lk(_mutex);
    for (auto& a : _acceptors)
    {
      // сначала закрываем, чтоб реконнект на другой ассептор не прошел 
      a->close();
    }

    for (auto& a : _acceptors)
    {
      // stop - снихронная операция
      a->stop(nullptr);
    }
    
    
    for (auto& s : _io_services)
    {
      s->stop();
    }
    
    for (auto& t : _threads)
    {
      t->join();
    }
    
    _acceptors.clear();
    _threads.clear();
    _io_services.clear();
    DEBUG_LOG_BEGIN("acceptor stop")

  }
  
private:
  
  void _reconfigure_and_start()
  {
    size_t threads = static_cast<size_t>(_opt.threads);
    
    while ( _threads.size() > threads )
    {
      _io_services.back()->stop();
      _acceptors.back()->stop(nullptr);
      _threads.back()->join();
      _acceptors.pop_back();
      _io_services.pop_back();
      _threads.pop_back();
    }
    
        
    typedef typename acceptor_type::descriptor_type descriptor_type;
    descriptor_type desc( _io_service );
    auto acceptor_proto = std::make_shared<acceptor_type>( std::move(desc), _opt);
    
    /// listen TODO: в ассептор
    
    acceptor_proto->listen();

    
    if ( threads == 0)
    {
      acceptor_proto->start();
      _acceptors.push_back( acceptor_proto );
      return;
    }

    auto itr = _acceptors.begin();
    for (size_t i = 0 ; i < threads; ++i)
    {
      if ( _acceptors.size() < threads )
      {
        auto io = std::make_shared<io_service_type>();
        _io_services.push_back(io);
        _acceptors.push_back( acceptor_proto->clone(*io));
      }
      else
      {
        (*itr)->reconfigure(_opt);
        ++itr;
      }
    }

    /// 
    /// start
    /// 
    
    for(auto& ptr : _acceptors)
    {
      ptr->start();
    }

    for (auto& s : _io_services)
    {
      _threads.push_back( std::make_shared<std::thread>( [s]()
      { 
        COMMON_LOG_MESSAGE("run acceptor thread")
        s->run();
      }));
    }

  }
  
private:
  io_service_type& _io_service;
  //mutable mutex_type _mutex;
  options_type _opt;
  //acceptor_options_type _acceptor_options;
  //acceptor_ptr _acceptor_proto;
  acceptor_list _acceptors;
  thread_list _threads;
  io_service_list _io_services;
};
  
template<typename Acceptor>
struct aspect: fas::aspect
<
  fas::advice<_before_stop_, ad_before_stop>,
  fas::group< ::wfc::io::_on_stop_, _before_stop_>, // TODO: переименовать
  fas::type< _acceptor_type_, Acceptor>,
  fas::value< _acceptors_, std::list< std::shared_ptr<Acceptor> > >,
  fas::value< _threads_, std::list< std::unique_ptr<std::thread> > >,
  fas::value< _io_services_, std::list< std::shared_ptr< ::wfc::io_service> > >,
  fas::advice<_configure_server_, ad_configure>,
  fas::group< _configure_, _configure_server_>,
  fas::advice<_start_server_, ad_start>,
  fas::group< ::wfc::io::_on_start_, _start_server_>
>
{};
 
template<typename A>  
class server
  : public basic_io<A>
  , public std::enable_shared_from_this< server<A> >
{
public:
  
  typedef basic_io<A> super;
  typedef typename super::options_type options_type;
  typedef typename super::aspect::template advice_cast< _acceptor_type_ >::type acceptor_type;
  typedef acceptor_manager<acceptor_type, options_type> acceptor_manager_type;
                        

  server( ::wfc::io_service& io, const options_type& conf)
    : super(io, conf)
    , _acceptor_manager( io, conf)
  {
    super::create(*this);
    //!!! this->get_aspect().template gete<_configure_>()(*this, conf);
  }

  /*
  void configure(const options_type& conf)
  {
    abort();
    //this->get_aspect().template gete<_configure_>()(*this, conf);
  }
  
  
  void reconfigure(const options_type& conf)
  {
    abort();
    //this->get_aspect().template get<_reconfigure_>()(*this, conf);
  }
  */
  
  void stop(std::function<void()> finalize)
  {
    typename super::lock_guard lk(super::mutex());
    _acceptor_manager.stop();
    super::stop(*this, finalize);
    /*
    super::get_io_service().reset();
    while ( 0!=super::get_io_service().poll() ) { super::get_io_service().reset();};
    */
  }
  
  void start()
  {
    typename super::lock_guard lk(super::mutex());
    super::start(*this);
    _acceptor_manager.start();
    
  }
private:
  acceptor_manager_type _acceptor_manager;
};
  
}}}
