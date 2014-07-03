#pragma once

#include <cstddef>

namespace wfc{ namespace gateway{ 
  
struct provider_config
{
  bool enabled = false;
  bool sequence_mode = true;
  size_t queue_limit = 1024*1024;
  size_t queue_warning = 1024*10;
};

}}

