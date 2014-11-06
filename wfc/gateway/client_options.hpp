#pragma once

namespace wfc{ namespace gateway{ 
  
template<typename T>
struct client_options: T 
{
  bool enabled = false;
  size_t count = 1;
};

}}