#pragma once

#include <comet/pubsub/pubsub_status.hpp>
#include <comet/pubsub/message.hpp>

namespace mamba{ namespace comet{

namespace request
{
  struct publish
    : message
  {
    std::string channel;
  };
}

namespace response
{
  struct publish
  {
    pubsub_status status;
  };
}

}}