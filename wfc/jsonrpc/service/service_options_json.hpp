//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wjrpc/engine/engine_options_json.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{

template<typename T>
struct service_options_json
{
  typedef T options_type;
  typedef typename options_type::engine_options engine_options;
  
  JSON_NAME(target)
  JSON_NAME(remove_outdated_ms)
  JSON_NAME(allow_non_jsonrpc)
  
  typedef json::object<
    options_type,
    json::member_list<
      json::member< n_target, options_type, std::string, &options_type::target_name>,
      json::member< n_allow_non_jsonrpc, options_type, bool, &options_type::allow_non_jsonrpc>,
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
