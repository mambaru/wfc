#pragma once

#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/strategy/posix/reader/sync_loop.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{ namespace rn{
  
struct sync_loop: fas::aspect<
  fas::alias< wfc::io::reader::loop::_outgoing_, wfc::io::rn::reader::_incoming_>,
  fas::alias< wfc::io::rn::reader::_outgoing_, wfc::io::reader::_outgoing_>,
  wfc::io::rn::reader::aspect,
  wfc::io::strategy::posix::reader::sync_loop
>{};

}}}}}}
