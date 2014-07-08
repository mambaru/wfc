#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/query.hpp>
#include <wfc/pubsub/api/types_json.hpp>
#include <wfc/pubsub/api/pubsub_status_json.hpp>

namespace wfc{ namespace pubsub{

namespace request
{
  struct query_json
  {
    FAS_NAME(channel)
    FAS_NAME(content)
    typedef
      json::object<
        query,
        fas::type_list_n<
          json::member<n_channel, query, std::string, &query::channel>,
          json::member<n_content, query, data_ptr,    &query::content,  
            json::pointer< data_ptr, json::raw_value<data_t> >
          >
        >::type
    > type;

    typedef type::serializer serializer;
  };
}

namespace response
{
  struct query_json
  {
    FAS_NAME(channel)
    FAS_NAME(status)
    FAS_NAME(content)
    typedef
      json::object<
        query,
        fas::type_list_n<
          json::member<n_status,  query, ::wfc::pubsub::status, &query::status, pubsub_status_json>,
          json::member<n_channel, query, std::string, &query::channel>,
          json::member<n_content, query, data_ptr,    &query::content,  
            json::pointer< data_ptr, json::raw_value<data_t> >
          >
        >::type
    > type;

    typedef type::serializer serializer;
  };
}

}}
