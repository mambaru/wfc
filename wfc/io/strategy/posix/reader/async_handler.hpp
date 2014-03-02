#pragma once

/*
#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>
#include <wfc/io/reader/errors/aspect.hpp>
*/
#include <wfc/io/async_read_some.hpp>

#include <wfc/io/reader/stream/async/aspect.hpp>
#include <wfc/io/reader/read/handler/aspect.hpp>
#include <wfc/io/strategy/posix/reader/basic.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct async_handler: fas::aspect<
  basic,
  //wfc::io::reader::stream::async::aspect,
  fas::advice< wfc::io::reader::stream::async::_incoming_, wfc::io::async_read_some<wfc::io::reader::common::_incoming_> >, 

  wfc::io::reader::read::handler::aspect,
  
  fas::alias< wfc::io::reader::read::handler::_outgoing_, wfc::io::reader::stream::async::_incoming_>,
  //fas::alias< wfc::io::reader::stream::async::_outgoing_, wfc::io::reader::common::_incoming_>,
  fas::alias< wfc::io::reader::common::_outgoing_, wfc::io::reader::_outgoing_>
>{};

}}}}}