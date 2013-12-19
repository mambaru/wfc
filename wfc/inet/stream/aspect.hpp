#pragma once

#include <wfc/inet/conn/ad_connection_start.hpp>
#include <wfc/inet/conn/ad_stream_reader.hpp>
#include <wfc/inet/conn/ad_stream_writer.hpp>
#include <wfc/inet/stream/tags.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <wfc/inet/types.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace stream{

struct ad_outgoing
{
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    t.get_aspect().template get<_writer_>()(t, std::move(d));
    // warning or close при проверки
  }
};

struct ad_on_write
{
  template<typename T>
  void operator()(T& /*t*/)
  {
   /* std::cout << "on_write outgoing size=" << 
      t.get_aspect().template get<_writer_>().size() << std::endl;
     */ 
  }
};
/*
 * _incoming_ required
 */
struct aspect: fas::aspect
< 
  fas::advice<_start_,    ad_connection_start<_reader_> >,
  //fas::advice<_reader_,   ad_stream_reader<_incoming_> >,
  fas::advice<_reader_,   ad_stream_reader<> >,
  fas::advice<_writer_,   ad_stream_writer<> >,
  fas::alias<_on_read_,   _incoming_>, 
  fas::alias<_input_,     _reader_>,
  fas::advice<_outgoing_,  ad_outgoing>,
  fas::alias<_write_,     _writer_>,
  
  fas::type< _socket_type_, boost::asio::ip::tcp::socket>, 

  fas::advice<_on_write_, ad_on_write>, 
  fas::stub<_write_error_>, 
  fas::stub<_read_error_>, 
  fas::stub<_alive_error_>
>
{};

}}}
