#pragma once

#include <wfc/inet/conn/ad_connection_start.hpp>
#include <wfc/inet/conn/ad_stream_reader.hpp>
#include <wfc/inet/conn/ad_stream_writer.hpp>
#include <wfc/inet/stream/tags.hpp>
#include <wfc/inet/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace stream{

/*
 * _incoming_ required
 */
struct aspect: fas::aspect
< 
  fas::advice<_start_,    ad_connection_start<_reader_> >,
  fas::advice<_reader_,   ad_stream_reader<_incoming_> >,
  fas::advice<_writer_,   ad_stream_writer>,
  
  fas::alias<_input_,     _reader_>,
  fas::alias<_outgoing_,  _writer_>,
  fas::alias<_write_,     _writer_>,
  
  fas::type< _socket_type_, boost::asio::ip::tcp::socket>
>
{};

}}}
