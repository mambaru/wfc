#pragma once

#include <wfc/io/reader/basic_options.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

struct basic_options
  //:  ::wfc::io::reader::basic_options
{
  size_t rn_limit_error   = 1024*1024;
  size_t rn_limit_warning = 2*1024*1024;
};

}}}}
