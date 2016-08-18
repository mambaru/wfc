#pragma once 

// #include <wfc/jsonrpc/options.hpp>
#include <wfc/json.hpp>
#include <wjrpc/engine/engine_options_json.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct gateway_options_json
{
  typedef T options_type;
  typedef typename options_type::engine_options engine_options;
  
  JSON_NAME(incoming_target)
  JSON_NAME(outgoing_target)
  JSON_NAME(incoming_reg)
  JSON_NAME(outgoing_reg)
  JSON_NAME(remove_outdated_ms)
  /*
  JSON_NAME(allow_non_jsonrpc)
  JSON_NAME(call_lifetime_ms)
  
  */
  
  typedef wfc::json::object<
    options_type,
    ::wfc::json::member_list<
      ::wfc::json::member< n_incoming_target, options_type, std::string, &options_type::incoming_target_name>,
      ::wfc::json::member< n_outgoing_target, options_type, std::string, &options_type::outgoing_target_name>,
      ::wfc::json::member< n_incoming_reg, options_type, bool, &options_type::incoming_reg>,
      ::wfc::json::member< n_outgoing_reg, options_type, bool, &options_type::outgoing_reg>,
      ::wfc::json::base< ::wjrpc::engine_options_json<engine_options> >,
      ::wfc::json::member< n_remove_outdated_ms, options_type, time_t, &options_type::remove_outdated_ms>
      /*,
      ::wfc::json::member< n_allow_non_jsonrpc, engine_options, bool, &engine_options::allow_non_jsonrpc>,
      ::wfc::json::member< n_call_lifetime_ms, engine_options, time_t, &engine_options::call_lifetime_ms>,
      
      */
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

}}