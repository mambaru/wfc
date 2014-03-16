#pragma once

#include <wfc/inet/conn/stream/aspect.hpp>
#include <wfc/inet/conn/stream/tags.hpp>
#include <wfc/inet/conn/ad_async_receive.hpp>
#include <wfc/inet/conn/stream/ad_reader.hpp>
#include <wfc/inet/conn/ad_send.hpp>
#include <wfc/inet/conn/ad_async_send.hpp>
#include <fas/aop.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace stream{ namespace tcp{

struct aspect: fas::aspect
<
  fas::advice<_async_receive_, ad_async_receive>,  
  fas::advice<_reader_,   ad_reader<> >,
  fas::advice<_send_, ad_send >,
  fas::advice<_async_send_, ad_async_send >,  
  fas::alias<_input_,     _reader_ >,
  fas::type< _socket_type_, boost::asio::ip::tcp::socket >, 
  wfc::inet::conn::stream::aspect
>
{};

}}}}}
