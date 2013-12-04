#pragma once

#include <comet/pubsub/pubsub_status.hpp>
#include <string>

namespace mamba{ namespace comet{

namespace request
{
  struct subscribe
  {
    std::string channel;
    
  };
}

namespace response
{
  struct subscribe
  {
    pubsub_status status;
  };
}

}}