#pragma once 

// #include <wfc/jsonrpc/options.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct service_options_json
{
  typedef T options_type;
  typedef typename options_type::engine_options engine_options;
  
  JSON_NAME(target)
  JSON_NAME(allow_non_jsonrpc)
  
  typedef wfc::json::object<
    options_type,
    wfc::json::member_list<
      wfc::json::member< n_target, options_type, std::string, &options_type::target>,
      wfc::json::member< n_allow_non_jsonrpc, engine_options, bool, &engine_options::allow_non_jsonrpc>
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

}}