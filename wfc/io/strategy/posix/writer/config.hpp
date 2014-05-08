#pragma once

#include <functional>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace writer{
  
struct config
{
  size_t output_buffer_size;
  std::function<void()> not_alive;
};

}}}}}