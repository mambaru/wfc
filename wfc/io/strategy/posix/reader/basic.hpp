#pragma once

#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>
#include <wfc/io/reader/common/aspect.hpp>
#include <wfc/io/reader/errors/aspect.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct basic: fas::aspect<
  context<>,
  wfc::io::basic::aspect,
  wfc::io::posix::aspect,
  wfc::io::reader::common::aspect,
  wfc::io::reader::errors::aspect
>{};

}}}}}