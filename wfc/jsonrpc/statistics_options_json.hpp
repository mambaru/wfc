//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <wfc/jsonrpc/statistics_options.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{


struct statistics_options_json
{
  JSON_NAME(interval_ms)
  JSON_NAME(handler_map)
  JSON_NAME(result_map)
  JSON_NAME(result_queue)

  typedef json::object<
    statistics_options,
    json::member_list<
      json::member<n_interval_ms, statistics_options, time_t, &statistics_options::interval_ms>,
      json::member<n_handler_map, statistics_options, std::string, &statistics_options::handler_map>,
      json::member<n_result_queue, statistics_options, std::string, &statistics_options::result_queue>
    >,
    json::strict_mode
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

}}
