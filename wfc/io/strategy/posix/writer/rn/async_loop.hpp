#pragma once

#include <wfc/io/rn/writer/aspect.hpp>
#include <wfc/io/strategy/posix/writer/async_loop.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace writer{ namespace rn{
  
struct async_loop: fas::aspect<
  fas::alias< wfc::io::writer::write::_outgoing_, wfc::io::rn::writer::_incoming_ >,
  fas::alias< wfc::io::rn::writer::_outgoing_, wfc::io::writer::basic::_write_>,
  wfc::io::rn::writer::aspect,
  wfc::io::strategy::posix::writer::async_loop
>{};

}}}}}}
