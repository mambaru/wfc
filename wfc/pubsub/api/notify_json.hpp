#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <wfc/pubsub/api/types_json.hpp>
#include <wfc/pubsub/api/pubsub_status_json.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct notify_json
  {
    FAS_NAME(channel)
    FAS_NAME(content)
    typedef
      json::object<
        notify,
        fas::type_list_n<
          json::member<n_channel, notify, std::string, &notify::channel>,
          json::member<n_content, notify, data_t, &notify::content, types_json::data_type>
        >::type
    > type;

    typedef type::serializer serializer;
  };
}

namespace response
{
  struct notify_json
  {
    FAS_NAME(channel)
    FAS_NAME(status)
    typedef
      json::object<
        notify,
        fas::type_list_n<
          json::member<n_status, notify, ::wfc::pubsub::status, &notify::status, pubsub_status_json>,
          json::member<n_channel, notify, std::string, &notify::channel>
        >::type
    > type;

    typedef type::serializer serializer;
  };
}

}}
