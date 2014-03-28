#pragma once

#include <wfc/inet/conn/ad_send_to.hpp>
#include <wfc/inet/conn/ad_async_send_to.hpp>
#include <wfc/inet/conn/stream/aspect.hpp>
#include <wfc/inet/conn/stream/tags.hpp>
#include <fas/aop.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace stream{ namespace udp {
  
struct aspect: fas::aspect
< 
  fas::stub<_reader_>,  
  fas::advice<_send_, ad_send_to >,
  fas::advice<_async_send_, ad_async_send_to >,
  fas::type< _socket_type_, boost::asio::ip::udp::socket >, 
  wfc::inet::conn::stream::aspect
>
{};

}}}}}
