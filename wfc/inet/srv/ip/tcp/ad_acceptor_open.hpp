#pragma once 

#include <efc/inet/srv/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
  
struct ad_acceptor_open
{
  template<typename T>
  void operator()(T& t, fas::tag<_startup_>)
  {
    typedef typename T::aspect::template advice_cast<_acceptor_type_>::type acceptor_type;
    acceptor_type acceptor = std::make_shared<acceptor_type>( t.get_io_service() );
 
    boost::asio::ip::tcp::resolver resolver( t.get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      t.server_context().host, 
      t.server_context().port
    });

    acceptor->open(endpoint.protocol());
    acceptor->set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    acceptor->bind(endpoint);
    acceptor->listen();
    
    t.get_aspect().template get<_acceptor_>() = acceptor;
  }
};

  
}}}}}
