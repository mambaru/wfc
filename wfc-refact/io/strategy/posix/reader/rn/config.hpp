#pragma once

#include <functional>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{ namespace rn{
  
struct config
{
  size_t input_buffer_size;
  std::function<void()> not_alive;
  size_t rn_limit_error;
  size_t rn_limit_warning;
  
  config()
    : input_buffer_size(8096)
    , rn_limit_error(0)
    , rn_limit_warning(0)
  {}
};

}}}}}}