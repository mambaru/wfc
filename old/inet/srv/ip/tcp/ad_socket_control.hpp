#pragma once 

#include <wfc/inet/srv/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
  
struct ad_socket_control
{  
  template<typename T>
  void operator()(T& /*t*/, typename T::socket_type& socket)
  {
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    socket.io_control(non_blocking_io);
  }
};
  
}}}}}
