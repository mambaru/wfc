#pragma once

#include <wfc/pubsub/status.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct notify
  {
    std::string channel;
    data_t content;
    
    notify()
    {}

    notify(const std::string& channel)
      : channel(channel)
    {}

    operator bool () const
    {
      return !channel.empty();
    }
  };
}

namespace response
{
  struct notify
  {
    ::wfc::pubsub::status status = ::wfc::pubsub::status::not_support;
    std::string channel;
    data_t content;
  };
}

}}
