#pragma once

#include <wfc/pubsub/status.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct query
  {
    std::string channel;
    //data_t content;
    data_ptr content;

    query()
    {}

    query(const std::string& channel)
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
  struct query
  {
    ::wfc::pubsub::status status = ::wfc::pubsub::status::not_support;
    std::string channel;
    // data_t content;
    data_ptr content;
  };
}

}}
