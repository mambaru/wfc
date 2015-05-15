#pragma once

#include <wfc/io/async_read_some.hpp>

#include <wfc/io/reader/stream/async/aspect.hpp>
#include <wfc/io/reader/read/async_callback/aspect.hpp>
#include <wfc/io/strategy/posix/reader/basic.hpp>
#include <wfc/io/async_read_some.hpp>


namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct async_read_callback: fas::aspect<
  basic,
  fas::advice< wfc::io::reader::stream::async::_incoming_, wfc::io::async_read_some<wfc::io::reader::common::_incoming_> >, 
//  wfc::io::reader::stream::async::aspect,
  wfc::io::reader::read::async_callback::aspect,
  fas::alias< wfc::io::reader::read::async_callback::_outgoing_, wfc::io::reader::stream::async::_incoming_>,
//  fas::alias< wfc::io::reader::stream::async::_outgoing_, wfc::io::reader::common::_incoming_>,
  fas::alias< wfc::io::reader::common::_outgoing_, wfc::io::reader::read::async_callback::_incoming_>
>{};

}}}}}