#pragma once
#include <wfc/io/writer/loop/aspect.hpp>
#include <wfc/io/writer/stream/sync/aspect.hpp>
#include <wfc/io/writer/stream/async/aspect.hpp>
#include <wfc/io/writer/write/aspect.hpp>
#include <wfc/io/strategy/posix/writer/basic.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace writer{
  
struct async_loop: fas::aspect<
  //fas::alias< wfc::io::writer::write::_outgoing_, wfc::io::writer::stream::sync::_incoming_>,
  fas::alias< wfc::io::writer::write::_outgoing_, wfc::io::writer::stream::async::_incoming_>,
  fas::alias< wfc::io::writer::stream::sync::_outgoing_,  wfc::io::writer::basic::_incoming_>,
  fas::alias< wfc::io::writer::stream::async::_outgoing_, wfc::io::writer::basic::_incoming_>,
  fas::alias< wfc::io::writer::basic::_outgoing_, wfc::io::writer::loop::_incoming_>,
  fas::alias< wfc::io::writer::loop::_write_some_, wfc::io::writer::stream::async::_incoming_>,
  fas::alias< wfc::io::writer::loop::_outgoing_,wfc::io::writer::_outgoing_>,
  fas::stub<wfc::io::writer::_outgoing_>,
  
  wfc::io::writer::loop::aspect,
  wfc::io::writer::stream::async::aspect,
  wfc::io::writer::stream::sync::aspect,
  wfc::io::writer::write::aspect,
  
  basic
>{};

}}}}}

