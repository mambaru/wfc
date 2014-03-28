#pragma once

#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/strategy/posix/reader/sync_handler.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{ namespace rn{
  
struct sync_handler: fas::aspect<
  fas::alias< wfc::io::reader::common::_outgoing_, wfc::io::rn::reader::_incoming_>,
  fas::alias< wfc::io::rn::reader::_outgoing_, wfc::io::reader::_outgoing_>,
  wfc::io::rn::reader::aspect,
  wfc::io::strategy::posix::reader::sync_handler
>{};

}}}}}}
