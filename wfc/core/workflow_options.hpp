#pragma once
#include <iow/workflow/queue_options.hpp>

namespace wfc{

struct workflow_options: 
  ::iow::queue_options
{
  bool enabled = true;
  int threads = 0;
  bool use_io_service = false;
};

}
