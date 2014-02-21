#pragma once

#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>
#include <wfc/io/reader/errors/aspect.hpp>
#include <wfc/io/reader/stream/async/aspect.hpp>
#include <wfc/io/reader/read/async/aspect.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct async_read: fas::aspect<
  context<>,
  //fas::alias< wfc::io::_incoming_, wfc::io::reader::read::async::_incoming_>,
  wfc::io::basic::aspect,
  wfc::io::posix::aspect,
  wfc::io::reader::common::aspect,
  wfc::io::reader::errors::aspect,
  wfc::io::reader::stream::async::aspect,
  wfc::io::reader::read::async::aspect
>{};

}}}}}