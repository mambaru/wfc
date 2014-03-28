#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <wfc/pubsub/api/message_json.hpp>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace pubsub{

struct pubsub_status_json
{
  FAS_NAME(ready)
  FAS_NAME(forbidden)
  FAS_NAME(bad_gateway)
  FAS_NAME(not_found)
  FAS_NAME(internal_error)
  FAS_NAME(not_support)
  FAS_NAME(wait)

  
  typedef fas::type_list_n<
    json::enum_value<n_ready,          pubsub::status, pubsub::status::ready>,
    json::enum_value<n_forbidden,      pubsub::status, pubsub::status::forbidden>,
    json::enum_value<n_bad_gateway,    pubsub::status, pubsub::status::bad_gateway>,
    json::enum_value<n_not_found,      pubsub::status, pubsub::status::not_found>,
    json::enum_value<n_internal_error, pubsub::status, pubsub::status::internal_error>,
    json::enum_value<n_not_support,    pubsub::status, pubsub::status::not_support>,
    json::enum_value<n_wait,           pubsub::status, pubsub::status::wait>
  >::type status_enum_list;

  typedef json::enumerator< pubsub::status, status_enum_list > type;
  
  typedef type::serializer serializer;
};

}}