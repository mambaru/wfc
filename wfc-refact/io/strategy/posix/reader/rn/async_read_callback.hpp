#pragma once

#include <wfc/io/rn/reader/aspect.hpp>
#include <wfc/io/strategy/posix/reader/async_read_callback.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{ namespace rn{
  
struct async_read_callback: fas::aspect<
  fas::alias< wfc::io::reader::common::_outgoing_, wfc::io::rn::reader::_incoming_>,
  fas::alias< wfc::io::rn::reader::_outgoing_, wfc::io::reader::read::async_callback::_incoming_>,
  wfc::io::rn::reader::aspect,
  wfc::io::strategy::posix::reader::async_read_callback
>{};

}}}}}}