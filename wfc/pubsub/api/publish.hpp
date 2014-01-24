#pragma once

#include <wfc/pubsub/pubsub_status.hpp>
#include <wfc/pubsub/api/message.hpp>

namespace wfc{ namespace pubsub{

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
    pubsub::status status;
  };
}

}}