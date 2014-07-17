#pragma once

#include <wfc/io_service.hpp>
#include <wfc/io/server/tags.hpp>
#include <wfc/io/basic_io.hpp>
#include <wfc/io/tags.hpp>

#include <list>
#include <thread>
#include <memory>

namespace wfc{ namespace io{ namespace server{
  
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
        auto io = std::make_shared<wfc::io_service>();
        services.push_back(io);
        acceptors.push_back( acceptor_proto->clone(*io));
      }
      else
      {
        (*itr)->reconfigure(conf);
      }
    }
    
    
    /*
    typedef typename acceptor_type::descriptor_type descriptor_type;

    

    // TODO: нахрен отсюда - я не знаю какой протокол
    boost::asio::ip::tcp::resolver resolver( t.get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      t.options().host, 
      t.options().port
    });

    desc.open(endpoint.protocol());
    desc.set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    desc.bind(endpoint);
    desc.listen(1024); // TODO: сделать опционально 
    
    COMMON_LOG_MESSAGE("listen " << t.options().host << ":" << t.options().port)

    auto itr = acceptors.begin();
    auto& services = t.get_aspect().template get<_io_services_>();
    for (size_t i = 0 ; i < static_cast<size_t>(t.options().threads); ++i, ++itr)
    {
      if ( acceptors.size() <  static_cast<size_t>(t.options().threads) )
      {
        auto io = std::make_shared<wfc::io_service>();
        typename descriptor_type::native_handle_type fd = ::dup(desc.native_handle());
        descriptor_type descriptor( *io, boost::asio::ip::tcp::v4(), fd);
        acceptors.push_back( std::make_shared<acceptor_type>( std::move(descriptor), conf) );
        services.push_back(io);
      }
      else
      {
        // TODO: сделать реконфигурацию
      }
    }
    
    if ( t.options().threads == 0)
    {
      acceptors.push_back( std::make_shared<acceptor_type>( std::move(desc), conf));
    }
    */
  }
};

struct ad_start
{
  //std::shared_ptr<wfc::io_service> io_service_ptr;

  template<typename T>
  void operator()(T& t)
  {
    
    //t.dispatch([&t]()
    //{
      
      
      auto& acceptors = t.get_aspect().template get<_acceptors_>();
      //COMMON_LOG_MESSAGE("listen " << t.options().host << ":" << t.options().port)
      //acceptors.front()->listen();
      
      for(auto& ptr : acceptors)
      {
        ptr->start();
      }

      auto& threads   = t.get_aspect().template get<_threads_>();
      threads.clear();
      auto& services = t.get_aspect().template get<_io_services_>();
      for (auto s : services)
      {
        threads.push_back( std::make_unique<std::thread>( [&t, s](){ 
          s->run();
        }));
      }
    //});
  }  
};

struct _before_stop_;
struct ad_before_stop
{
  //std::shared_ptr<wfc::io_service> io_service_ptr;

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
  
template<typename Acceptor>
struct aspect: fas::aspect
<
  fas::advice<_before_stop_, ad_before_stop>,
  fas::group< wfc::io::_on_stop_, _before_stop_>, // TODO: переименовать
  fas::type< wfc::io::server::_acceptor_type_, Acceptor>,
  fas::value< _acceptors_, std::list< std::shared_ptr<Acceptor> > >,
  fas::value< _threads_, std::list< std::unique_ptr<std::thread> > >,
  fas::value< _io_services_, std::list< std::shared_ptr<wfc::io_service> > >,
  fas::advice<_configure_server_, ad_configure>,
  fas::group< _configure_, _configure_server_>,
  fas::advice<_start_server_, ad_start>,
  fas::group< wfc::io::_on_start_, _start_server_>
  
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

  server(wfc::io_service& io, const options_type& conf)
    : super(io, conf)
    // , _handler(conf.incoming_handler) // TODO: убрать
  {
    super::create(*this);
    this->get_aspect().template gete<_configure_>()(*this, conf);
  }

  
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
  
  
  void stop(std::function<void()> finalize)
  {
    typename super::lock_guard lk(super::mutex());
    super::stop(*this, finalize);
    // TODO: Убрать!!!
    super::get_io_service().reset();
    while ( 0!=super::get_io_service().poll() ) { super::get_io_service().reset();};
  }
  
  void start()
  {
    typename super::lock_guard lk(super::mutex());
    //this->get_aspect().template gete<_configure_>()(*this, super::options() );
    super::start(*this);
  }
  
  //wfc::io::incoming_handler_t _handler;
};
  
}}}
