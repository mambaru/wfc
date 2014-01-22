#pragma once

#include <wfc/pubsub/pubsub_status.hpp>
#include <string>

namespace wfc{

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

}