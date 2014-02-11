#pragma once

#include <wfc/inet/context.hpp>
#include <wfc/inet/conn/ad_connection_start.hpp>
#include <wfc/inet/stream/ad_outgoing.hpp>
#include <wfc/inet/conn/ad_stream_writer.hpp>
#include <wfc/inet/stream/ad_on_write.hpp>
#include <wfc/inet/stream/ad_shutdown.hpp>
#include <wfc/inet/conn/connection_context.hpp>
#include <wfc/inet/stream/context.hpp>


namespace wfc{ namespace inet{ namespace stream{


struct aspect: fas::aspect
< 
  ::wfc::inet::context<empty_context>,
  basic_context_t<context>, 
  fas::advice<_start_,    ad_connection_start<_reader_> >,
  fas::alias<_on_read_,   _incoming_ >,
  fas::advice<_outgoing_, ad_outgoing >,
  fas::alias<_write_,     _writer_ >,  
  fas::advice<_writer_,   ad_stream_writer<> >,
  fas::advice<_on_write_, ad_on_write>, 
  fas::advice<_shutdown_, ad_shutdown>,
  fas::stub<_write_error_>, 
  fas::stub<_read_error_>, 
  fas::stub<_alive_error_>
>
{};

}}}
