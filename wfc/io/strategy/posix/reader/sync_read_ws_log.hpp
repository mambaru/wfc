#pragma once

#include <wfc/io/strategy/posix/reader/sync_read.hpp>
#include <wfc/io/reader/common/trace/aspect.hpp>
#include <wfc/io/reader/errors/log/aspect.hpp>


namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct sync_read_ws_log: fas::aspect<
  sync_read,
  wfc::io::reader::errors::log::aspect,
  wfc::io::reader::common::trace::aspect

>{};

}}}}}