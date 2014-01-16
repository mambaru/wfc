#pragma once

#include <boost/asio.hpp>

namespace wfc{ namespace inet{

struct ad_tcp_socket
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  
  template<typename T>
  void operator()(T& /*t*/, socket_type& socket)
  {
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    socket.io_control(non_blocking_io);
  }
};

struct ad_udp_socket
{
  typedef ::boost::asio::ip::udp::socket socket_type;
  
  template<typename T>
  void operator()(T& /*t*/, socket_type& socket)
  {
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    socket.io_control(non_blocking_io);
  }
};

}}