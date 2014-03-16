#pragma once

#include <wfc/io_service.hpp>
#include <wfc/io/server/tags.hpp>
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
    
    /*
    t.get_aspect().template get<_acceptor_count_>() = conf.acceptors;
    t.get_aspect().template get<_thread_count_>() = conf.threads;
    */
    
    auto& acceptors = t.get_aspect().template get<_acceptors_>();
    while ( acceptors.size() > conf.threads )
    {
      //! acceptors.back()->stop();
      acceptors.pop_back();
    }
    
    typedef typename acceptor_type::descriptor_type descriptor_type;
  
    descriptor_type desc( t.get_io_service() );
  
    boost::asio::ip::tcp::resolver resolver( t.get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      t.options().host, 
      t.options().port
    });

    desc.open(endpoint.protocol());
    desc.set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    desc.bind(endpoint);
    desc.listen();

    

    auto itr = acceptors.begin();
    auto& services = t.get_aspect().template get<_io_services_>();
    for (size_t i =0 ; i < t.options().threads; ++i, ++itr)
    {
      if ( acceptors.size() < t.options().threads )
      {
        auto io = std::make_shared<wfc::io_service>();
        typename descriptor_type::native_handle_type fd = ::dup(desc.native_handle());
        descriptor_type descriptor( *io, boost::asio::ip::tcp::v4(), fd);
        
        //descriptor_type descriptor( t.get_io_service(), boost::asio::ip::tcp::v4(), fd);
        acceptors.push_back( std::make_unique<acceptor_type>( std::move(descriptor), conf, t._handler) );
        services.push_back(io);
      }
      else
      {
        // TODO: сделать реконфигурацию
        // (*itr)->create( conf );
      }
    }
  }
};

struct ad_start
{
  std::shared_ptr<wfc::io_service> io_service_ptr;

  template<typename T>
  void operator()(T& t)
  {
    // Ахтунг! сделать свой io_service если потоков > 0;
    t.dispatch([&t]()
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
        threads.push_back( std::make_unique<std::thread>( [&t, s](){ 
          s->run();
        }));
      }
    });
  }  
  /*
  template<typename T>
  void operator()(T& t)
  {
    // Ахтунг! сделать свой io_service если потоков > 0;
    t.dispatch([&t]()
    {
      size_t thread_count = t.get_aspect().template get<_thread_count_>();
      
      auto& acceptors = t.get_aspect().template get<_acceptors_>();
      auto& threads   = t.get_aspect().template get<_threads_>();
      
      while ( threads.size() > thread_count )
        threads.pop_back();
      
      while ( threads.size() < thread_count )
      {
        threads.push_back( std::make_unique<std::thread>( [&t](){ 
          t.get_io_service().run();
        }));
      }
      
      for(auto& ptr : acceptors)
      {
        ptr->start();
      }
    });
  }
  */
};
  
template<typename Acceptor>
struct aspect: fas::aspect
<
  //context<>,
  fas::stub< wfc::io::_stop_>,
  // fas::value<_acceptor_count_, size_t>,
  // fas::value<_thread_count_, size_t>,
  fas::type< wfc::io::server::_acceptor_type_, Acceptor>,
  fas::value< _acceptors_, std::list< std::unique_ptr<Acceptor> > >,
  fas::value< _threads_, std::list< std::unique_ptr<std::thread> > >,
  fas::value< _io_services_, std::list< std::shared_ptr<wfc::io_service> > >,
  fas::advice<_configure_server_, ad_configure>,
  fas::group< _configure_, _configure_server_>,
  fas::advice<_start_server_, ad_start>,
  fas::group< wfc::io::_start_, _start_server_>
  
>
{};
 
template<typename A>  
class server
  //: public fas::aspect_class<A>
  : public basic_io<A>
{
public:
  //typedef fas::aspect_class<A> super;
  //typedef typename super::aspect::template advice_cast< wfc::io::_options_type_>::type options_type;
  
  typedef basic_io<A> super;
  typedef typename super::options_type options_type;

  /*
  server(wfc::io_service& io, wfc::io::handler handler = nullptr)
    : io_service(io)
    , _handler(handler)
  {
    this->get_aspect().template get<_create_>()(*this);
  }
  */
  
  
  server(wfc::io_service& io, const options_type& conf, wfc::io::handler handler = nullptr)
    : super(io, conf)
    , _handler(handler)
  {
    super::create(*this);
    //this->get_aspect().template get<_create_>()(*this);
    this->get_aspect().template gete<_configure_>()(*this, conf);
  }

  
  void configure(const options_type& conf)
  {
    this->get_aspect().template gete<_configure_>()(*this, conf);
  }
  
  
  void reconfigure(const options_type& conf)
  {
    this->get_aspect().template get<_reconfigure_>()(*this, conf);
  }
  
  
  void stop()
  {
    super::stop(*this);
    //this->get_aspect().template get<_stop_>()(*this);
  }
  
  void start()
  {
    super::start(*this);
    //this->get_aspect().template get<_start_>()(*this);
  }
  
  
  /*
  wfc::io_service& get_io_service()
  {
    return io_service;
  }
  */
  
  //wfc::io_service& io_service;
  wfc::io::handler _handler;
  
};
  
}}}