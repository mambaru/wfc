#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/describe.hpp>
#include <wfc/pubsub/api/pubsub_status_json.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct describe_json
  {
    JSON_NAME(channel)
    
    typedef json::member_value<
      describe,
      describe, 
      std::string, 
      &describe::channel
    > type;

    typedef type::serializer serializer;
  };
}

namespace response
{
  struct describe_json
  {
    typedef json::member_value<
      describe,
      describe, 
      pubsub::status, 
      &describe::status, 
      pubsub_status_json::type
    > type;
    
    typedef type::serializer serializer;
  };
}

}}
