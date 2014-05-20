#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/load.hpp>
#include <wfc/pubsub/api/topic_json.hpp>
#include <wfc/pubsub/api/pubsub_status_json.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct load_json
  {
    FAS_NAME(channel)
    FAS_NAME(cursor)
    FAS_NAME(limit)
    FAS_NAME(load_params)
    typedef
      json::object<
        load,
        fas::type_list_n<
          json::member<n_channel, load, std::string, &load::channel>,
          json::member<n_cursor, load, cursor_t, &load::cursor>,
          json::member<n_limit, load, size_t, &load::limit>,
          json::member<n_load_params, load, std::string, &load::load_params>
        >::type
    > type;

    typedef type::serializer serializer;
  };
}

namespace response
{
  struct load_json
  {
    FAS_NAME(status)
    typedef
      json::object<
        load,
        fas::type_list_n<
          json::member<n_status, load, ::wfc::pubsub::status, &load::status, pubsub_status_json>,
          json::base< topic_json::type >
        >::type
    > type;

    typedef type::serializer serializer;
  };
}

}}
