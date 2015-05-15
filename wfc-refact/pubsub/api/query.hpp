#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct query
  {
    std::string channel;
    data_ptr content;
    
    query()
      : channel()
      , content(nullptr)
    {}

    query(const query& other)
      : channel(other.channel)
    {
      if (other.content != nullptr)
        content = std::make_unique<data_t>(other.content->begin(), other.content->end());
    }

    query(query&& other)
      : channel( std::move(other.channel) )
      , content( std::move(other.content) )
    {
    }

    query& operator=(const query& other)
    {
      channel = other.channel;
      if (other.content != nullptr)
        content = std::make_unique<data_t>(other.content->begin(), other.content->end());
      return *this;
    }

    query& operator=(query&& other)
    {
      channel = std::move(other.channel);
      content = std::move(other.content);
      return *this;
    }

  };
}

namespace response
{
  struct query
  {
    ::wfc::pubsub::status status = ::wfc::pubsub::status::not_support;
    std::string channel;
    data_ptr content;
  };
}

}}
