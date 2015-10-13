#pragma once 

// #include <wfc/jsonrpc/options.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct gateway_options_json
{
  typedef T options_type;
  typedef typename options_type::engine_options engine_options;
  
  JSON_NAME(incoming_target)
  JSON_NAME(outgoing_target)
  JSON_NAME(direct_mode)
  
  typedef wfc::json::object<
    options_type,
    ::wfc::json::member_list<
      ::wfc::json::member< n_incoming_target, options_type, std::string, &options_type::incoming_target>,
      ::wfc::json::member< n_outgoing_target, options_type, std::string, &options_type::outgoing_target>,
      ::wfc::json::member< n_direct_mode, engine_options, bool, &engine_options::direct_mode>
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

}}