#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/subscribe.hpp>
#include <wfc/pubsub/api/pubsub_status_json.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct subscribe_json
  {
    JSON_NAME(channel)
    
    /*
    typedef
      json::object<
        subscribe,
        fas::type_list_n<
          json::member<n_channel, subscribe, std::string, &subscribe::channel>
        >::type
    > type;
    */

    typedef json::member_value<
      subscribe,
      subscribe, 
      std::string, 
      &subscribe::channel/*, 
      pubsub_status_json::type*/
    > type;

    typedef type::serializer serializer;
  };
}

namespace response
{
  struct subscribe_json
  {
    typedef json::member_value<
      subscribe,
      subscribe, 
      pubsub::status, 
      &subscribe::status, 
      pubsub_status_json::type
    > type;
    
    typedef type::serializer serializer;
  };
}

}}
