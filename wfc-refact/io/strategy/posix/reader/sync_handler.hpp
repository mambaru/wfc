#pragma once

#include <wfc/io/reader/stream/sync/aspect.hpp>
#include <wfc/io/reader/read/handler/aspect.hpp>
#include <wfc/io/strategy/posix/reader/basic.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct sync_handler: fas::aspect<
  basic,
  wfc::io::reader::stream::sync::aspect,
  wfc::io::reader::read::handler::aspect,
  fas::alias< wfc::io::reader::read::handler::_outgoing_, wfc::io::reader::stream::sync::_incoming_>,
  fas::alias< wfc::io::reader::stream::sync::_outgoing_, wfc::io::reader::common::_incoming_>,
  fas::alias< wfc::io::reader::common::_outgoing_, wfc::io::reader::_outgoing_>
>{};

}}}}}