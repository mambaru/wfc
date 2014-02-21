#pragma once

#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>
#include <wfc/io/reader/errors/aspect.hpp>
#include <wfc/io/reader/stream/sync/aspect.hpp>
#include <wfc/io/reader/read/sync/aspect.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct sync_read: fas::aspect<
  context<>,
  wfc::io::basic::aspect,
  wfc::io::posix::aspect,
  wfc::io::reader::common::aspect,
  wfc::io::reader::errors::aspect,
  wfc::io::reader::stream::sync::aspect,
  wfc::io::reader::read::sync::aspect
>{};

}}}}}