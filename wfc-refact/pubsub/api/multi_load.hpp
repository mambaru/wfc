#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/load.hpp>
#include <wfc/memory.hpp>
#include <vector>

namespace wfc{ namespace pubsub{

namespace request
{
  typedef std::vector< load > multi_load;
}

namespace response
{
  typedef std::vector< load > multi_load;
}

}}
