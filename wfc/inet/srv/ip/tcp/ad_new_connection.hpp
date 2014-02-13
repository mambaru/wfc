#pragma once 

#include <efc/inet/srv/tags.hpp>
#include <efc/inet/srv/connection_manager.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
  
struct ad_new_connection
{
  template<typename T>
  void operator()(T& t, std::shared_ptr< typename T::socket_type> sock)
  {
    typedef typename T::connection_type connection_type;

    std::shared_ptr<connection_type> pconn = t.create_connection(
      sock, 
      [this, &t](std::shared_ptr<connection_type> pconn)->void
      {
        t.connection_manager()->erase(pconn);
      }
    );

    pconn->start();

    t.connection_manager()->insert(pconn);
  }
};
  
}}}}}
