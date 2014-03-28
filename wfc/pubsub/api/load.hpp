#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/topic.hpp>
#include <wfc/pubsub/api/types.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct load
  {
    std::string channel;
    cursor_t cursor = cursor_t();
    size_t limit = 0;
    std::string load_params;
  };
}

namespace response
{
  struct load
    : topic
  {
    ::wfc::pubsub::status status = ::wfc::pubsub::status::not_support;
  };
}

}}