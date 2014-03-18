#pragma once

#include <wfc/pubsub/status.hpp>
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
    ::wfc::pubsub::status status = ::wfc::pubsub::status::not_support;
  };
}

}}
