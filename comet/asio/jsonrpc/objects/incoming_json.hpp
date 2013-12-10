#pragma once

#include <comet/json/name.hpp>
#include <comet/json/json.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct incoming_json
{
  typedef incoming::pair_type pair_type;
  typedef json::raw_pair<pair_type> pair_json;
  typedef json::pointer< std::unique_ptr<int>, json::value<int> > id_json;
  
  NAME(id);
  NAME(method);
  NAME(params);
  NAME(result);
  NAME(error);
  
  typedef json::object<
    incoming, 
    fas::type_list_n<
      json::member<n_method, incoming, pair_type,            &incoming::method, pair_json>,
      json::member<n_params, incoming, pair_type,            &incoming::params, pair_json>,
      json::member<n_result, incoming, pair_type,            &incoming::result, pair_json>,
      json::member<n_error,  incoming, pair_type,            &incoming::error,  pair_json>,
      json::member<n_id,     incoming, std::unique_ptr<int>, &incoming::id,     id_json>
    >::type
  > type;

  typedef type::serializer serializer;
};

}}}}


