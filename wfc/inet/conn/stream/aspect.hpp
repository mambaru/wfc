#pragma once

#include <wfc/inet/context.hpp>
#include <wfc/inet/basic_context_class.hpp>
#include <wfc/inet/conn/ad_connection_start.hpp>
#include <wfc/inet/conn/ad_do_active.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <wfc/inet/conn/stream/ad_outgoing.hpp>
#include <wfc/inet/conn/stream/ad_writer.hpp>
#include <wfc/inet/conn/stream/ad_on_write.hpp>
#include <wfc/inet/conn/stream/ad_shutdown.hpp>
#include <wfc/inet/conn/basic_context.hpp>
#include <wfc/inet/conn/stream/basic_context.hpp>

#include <functional>

namespace wfc{ namespace inet{ namespace conn{ namespace stream{


// Перенести часть в connection
struct aspect: fas::aspect
< 
  ::wfc::inet::context< /*empty_context*/ >,
  basic_context_class<basic_context>, 
  fas::advice<_start_,    ad_connection_start<_reader_> >,
  fas::advice<_do_active_, ad_do_active>,
  fas::group<_on_read_,   _do_active_ >,
  //fas::alias<_on_read_,   _incoming_ >,
  fas::advice<_outgoing_, ad_outgoing >,
  fas::alias<_write_,     _writer_ >,  
  fas::advice<_writer_,   ad_writer<> >,
  fas::advice<_on_write_, ad_on_write>, 
  fas::advice<_shutdown_, ad_shutdown>,
  fas::stub<_write_error_>, 
  fas::stub<_read_error_>, 
  fas::stub<_alive_error_>,
  fas::value<_activity_, std::function<void()> >
  
>
{};

}}}}
