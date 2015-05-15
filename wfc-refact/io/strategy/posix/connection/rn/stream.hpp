#pragma once


#include <wfc/io/strategy/posix/reader/rn/async_loop.hpp>
#include <wfc/io/strategy/posix/reader/rn/log.hpp>
#include <wfc/io/strategy/posix/reader/rn/trace.hpp>

#include <wfc/io/strategy/posix/writer/rn/async_loop.hpp>
#include <wfc/io/strategy/posix/writer/log.hpp>
#include <wfc/io/strategy/posix/writer/trace.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace connection{ namespace rn{
  
struct stream: fas::aspect<
  fas::alias< wfc::io::reader::_outgoing_, wfc::io::_incoming_>,
  fas::alias< wfc::io::_outgoing_, wfc::io::writer::_incoming_ >,
  
  wfc::io::strategy::posix::reader::rn::async_loop,
  wfc::io::strategy::posix::reader::rn::log,
  wfc::io::strategy::posix::reader::rn::trace,
  wfc::io::strategy::posix::writer::rn::async_loop,
  wfc::io::strategy::posix::writer::log,
  wfc::io::strategy::posix::writer::trace
>{};

}}}}}}
