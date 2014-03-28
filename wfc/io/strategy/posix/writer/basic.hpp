#pragma once

#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/posix/aspect.hpp>
#include <wfc/io/writer/basic/aspect.hpp>
#include <wfc/io/writer/errors/aspect.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace writer{
  
struct basic: fas::aspect<
  context<>,
  wfc::io::basic::aspect,
  wfc::io::posix::aspect,
  wfc::io::writer::basic::aspect,
  wfc::io::writer::errors::aspect
>{};

}}}}}