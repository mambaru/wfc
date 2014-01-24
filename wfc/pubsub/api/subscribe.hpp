#pragma once

#include <wfc/pubsub/pubsub_status.hpp>
#include <string>

namespace wfc{ namespace pubsub{

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
    pubsub::status status;
  };
}

}}
