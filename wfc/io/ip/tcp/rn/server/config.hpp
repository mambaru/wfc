#pragma once

#include <cstddef>
#include <functional>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace impl{ 
  
struct config
{
  size_t acceptors = 1;
  size_t threads = 0;
  
  std::function<void()> not_alive = nullptr;
};
  
}}}}}}