#pragma once 

#include <string>
#include <wfc/inet/client/tags.hpp>


namespace wfc{ namespace inet{
  
struct ad_connect
{
  template<typename T>
  void operator()(T& t, fas::tag<_start_> )
  {
    t.get_aspect().template get<_connection_manager_>() = std::make_shared<connection_manager<>>();
    
    typedef typename T::connection_type connection_type;
    typedef typename T::socket_type socket_type;
    std::shared_ptr<socket_type> sock = std::make_shared<socket_type>( t.get_io_service() );
    
    boost::asio::ip::tcp::resolver resolver( t.get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      t.client_context().host, 
      t.client_context().port
    });
    
    sock->connect( endpoint );
    std::shared_ptr<connection_type> conn = t.create_connection(sock, [&t](std::shared_ptr<connection_type> conn){
        std::cout << "closed" << std::endl;
        t.get_aspect().template get<_connection_manager_>()->erase(conn);
    });
    std::cout << "connected" << std::endl;
    t.get_aspect().template get<_connection_manager_>()->insert(conn);
    conn->start();
    
  }
};


}}