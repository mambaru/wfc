#pragma once

#include <functional>

namespace wfc{ namespace io{ namespace strategy{ namespace posix{ namespace reader{
  
struct config
{
  size_t input_buffer_size;
  std::function<void()> not_alive;
};

}}}}}