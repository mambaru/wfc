#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <wfc/pubsub/api/message_json.hpp>
#include <wfc/pubsub/api/pubsub_status_json.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct publish_json
  {
    FAS_NAME(channel)
    typedef
      json::object<
        publish,
        fas::type_list_n<
          json::member<n_channel, publish, std::string, &publish::channel>, 
          json::base< message_json::type >
        >::type
    > type;

    typedef type::serializer serializer;
  };
}

namespace response
{
  struct publish_json
  {
    typedef json::member_value<
      publish,
      publish, 
      pubsub::status, 
      &publish::status, 
      pubsub_status_json::type
    > type;
    
    typedef type::serializer serializer;
  };
}

}}
