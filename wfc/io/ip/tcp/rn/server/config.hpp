#pragma once

#include <cstddef>
#include <functional>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace impl{ 
  
struct config
{
  size_t acceptors = 1;
  size_t threads = 0;
  
  size_t output_buffer_size = 1024;
  size_t input_buffer_size = 1024;
  std::function<void()> not_alive = nullptr;
  std::function<void()> release_function = nullptr;

};
  
}}}}}}