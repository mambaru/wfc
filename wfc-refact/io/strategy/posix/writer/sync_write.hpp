#pragma once

#include <wfc/io/writer/stream/sync/aspect.hpp>
#include <wfc/io/writer/write/aspect.hpp>
#include <wfc/io/strategy/posix/writer/basic.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace writer{
  
struct sync_write: fas::aspect<
/*
  fas::alias< wfc::io::writer::write::_outgoing_, wfc::io::writer::stream::sync::_incoming_>,
  fas::alias< wfc::io::writer::stream::sync::_outgoing_, wfc::io::writer::basic::_incoming_>,
  fas::alias< wfc::io::writer::basic::_outgoing_, wfc::io::writer::write::_incoming_>,
*/
  fas::alias< wfc::io::writer::write::_outgoing_, wfc::io::writer::basic::_write_>,
  fas::alias< wfc::io::writer::basic::_write_some_, wfc::io::writer::stream::sync::_incoming_>,
  fas::alias< wfc::io::writer::stream::sync::_outgoing_, wfc::io::writer::basic::_incoming_>,
  fas::alias< wfc::io::writer::basic::_outgoing_, wfc::io::writer::write::_incoming_>,
  
  wfc::io::writer::write::aspect,
  wfc::io::writer::stream::sync::aspect,
  basic
>{};

}}}}}
