#pragma once

#include <wfc/pubsub/status.hpp>
#include <string>

namespace wfc{ namespace pubsub{

namespace request
{
  struct describe
  {
    std::string channel;
  };
}

namespace response
{
  struct describe
  {
    ::wfc::pubsub::status status = ::wfc::pubsub::status::not_support;
  };
}

}}
