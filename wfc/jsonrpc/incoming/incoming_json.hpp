#pragma once

#include <wfc/jsonrpc/incoming/incoming.hpp>
#include <wfc/json/json.hpp>
#include <fas/type_list.hpp>


namespace wfc{ namespace jsonrpc{

struct incoming_json
{
  typedef incoming::pair_type pair_type;
  typedef json::raw_pair<pair_type> pair_json;
  
  JSON_NAME(id)
  JSON_NAME(method)
  JSON_NAME(params)
  JSON_NAME(result)
  JSON_NAME(error)
  
  typedef json::object<
    incoming, 
    fas::type_list_n<
      json::member<n_method, incoming, pair_type, &incoming::method, pair_json>,
      json::member<n_params, incoming, pair_type, &incoming::params, pair_json>,
      json::member<n_result, incoming, pair_type, &incoming::result, pair_json>,
      json::member<n_error,  incoming, pair_type, &incoming::error,  pair_json>,
      json::member<n_id,     incoming, pair_type, &incoming::id,     pair_json>
    >::type
  > type;

  typedef type::serializer serializer;
};

  
}}
