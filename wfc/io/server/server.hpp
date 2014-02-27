#pragma once

#include <wfc/io_service.hpp>
#include <wfc/io/server/tags.hpp>

#include <thread>
#include <memory>

namespace wfc{ namespace io{ namespace server{
  
struct _configure_server_;
struct _start_server_;
  
struct ad_configure
{
  template<typename T, typename Conf>
  void operator()(T& t, const Conf& conf)
  {
    typedef typename T::aspect::template advice_cast<_acceptor_type_>::type acceptor_type;
    std::cout << "configure" << std::endl;
    t.get_aspect().template get<_acceptor_count_>() = conf.acceptors;
    t.get_aspect().template get<_thread_count_>() = conf.threads;
    
    auto& acceptors = t.get_aspect().template get<_acceptors_>();
    while ( acceptors.size() > conf.acceptors )
    {
      //! acceptors.back()->stop();
      acceptors.pop_back();
    }
    
    typedef typename acceptor_type::descriptor_type descriptor_type;
  
    descriptor_type desc( t.get_io_service() );
  
    boost::asio::ip::tcp::resolver resolver( t.get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      "0.0.0.0", 
      "12345"
    });

    desc.open(endpoint.protocol());
    desc.set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    desc.bind(endpoint);
    desc.listen();

    

    auto itr = acceptors.begin();
    for (size_t i =0 ; i < conf.acceptors; ++i, ++itr)
    {
      if ( acceptors.size() < conf.acceptors )
      {
        std::cout << "listen" << std::endl;
        //typename acceptor_type::descriptor_type descriptor(t.get_io_service());
        acceptors.push_back( std::make_unique<acceptor_type>( std::move(desc), conf) );
        break;
      }
      else
      {
        (*itr)->create( conf );
      }
    }
  }
};

struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "start" << std::endl;
    //size_t acceptor_count = t.get_aspect().template get<_acceptor_count_>();
    size_t thread_count = t.get_aspect().template get<_thread_count_>();
    
    auto& acceptors = t.get_aspect().template get<_acceptors_>();
    auto& threads   = t.get_aspect().template get<_threads_>();
    
    while ( threads.size() > thread_count )
      threads.pop_back();

    while ( threads.size() < thread_count )
    {
      threads.push_back( std::make_unique<std::thread>( [&t](){ t.get_io_service().run();}) );
    }
    
    for(auto& ptr : acceptors)
    {
      std::cout << "start" << std::endl;
      ptr->start();
    }
  }
};
  
template<typename Acceptor>
struct aspect: fas::aspect
<
  fas::value<_acceptor_count_, size_t>,
  fas::value<_thread_count_, size_t>,
  fas::type< wfc::io::server::_acceptor_type_, Acceptor>,
  fas::value< _acceptors_, std::list< std::unique_ptr<Acceptor> > >,
  fas::value< _threads_, std::list< std::unique_ptr<std::thread> > >,
  fas::advice<_configure_server_, ad_configure>,
  fas::group< _configure_, _configure_server_>,
  fas::advice<_start_server_, ad_start>,
  fas::group< _start_, _start_server_>
  
>
{};
 
template<typename A>  
class server
  : public fas::aspect_class<A>
{
public:
  typedef fas::aspect_class<A> super;

  server(wfc::io_service& io): io_service(io)
  {
    this->get_aspect().template get<_create_>()(*this);
  }
  
  template<typename Conf>
  server(wfc::io_service& io, const Conf& conf)
    : io_service(io)
  {
    this->get_aspect().template get<_create_>()(*this);
    this->get_aspect().template gete<_configure_>()(*this, conf);
  }

  template<typename Conf>
  void configure(const Conf& conf)
  {
    this->get_aspect().template gete<_configure_>()(*this, conf);
  }
  
  template<typename Conf>
  void reconfigure(const Conf& conf)
  {
    this->get_aspect().template get<_reconfigure_>()(*this, conf);
  }
  
  void stop()
  {
    this->get_aspect().template get<_stop_>()(*this);
  }
  
  void start()
  {
    this->get_aspect().template get<_start_>()(*this);
  }
  
  wfc::io_service& get_io_service()
  {
    return io_service;
  }
  
  wfc::io_service& io_service;
  
};
  
}}}