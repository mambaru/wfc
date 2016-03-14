#pragma once
#include <cstddef>

namespace wfc{

struct workflow_options
{
  size_t maxsize = 0;
  size_t wrnsize = 0;
  int threads = 0;
};

}
