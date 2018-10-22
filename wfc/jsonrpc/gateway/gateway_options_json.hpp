//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

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
  
  typedef json::object<
    options_type,
    json::member_list<
      json::member< n_incoming_target, options_type, std::string, &options_type::incoming_target_name>,
      json::member< n_outgoing_target, options_type, std::string, &options_type::outgoing_target_name>,
      json::member< n_incoming_reg, options_type, bool, &options_type::incoming_reg>,
      json::member< n_outgoing_reg, options_type, bool, &options_type::outgoing_reg>,
      json::base< ::wjrpc::engine_options_json<engine_options> >,
      json::member< n_remove_outdated_ms, options_type, time_t, &options_type::remove_outdated_ms>
    >,
    json::strict_mode
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

}}
