#pragma once

#include <wfc/pubsub/status.hpp>
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
    ::wfc::pubsub::status status = ::wfc::pubsub::status::not_support;
  };
}

}}