#pragma once
#include <iow/workflow/queue_options.hpp>

namespace wfc{

struct workflow_options: 
  ::iow::queue_options
{
  int threads = 0;
};

}