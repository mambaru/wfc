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
    cursor_t cursor;
    size_t limit;

    std::string load_params;

    load()
      : cursor()
      , limit(0)
  {}

  };
}

namespace response
{
  struct load
    : topic
  {
    pubsub::status status;
    load()
      : topic()
      , status( pubsub::status::not_impl )
    {}
  };
}

}}