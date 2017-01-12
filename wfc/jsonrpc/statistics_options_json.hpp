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

  typedef wfc::json::object<
    statistics_options,
    wfc::json::member_list<
      wfc::json::member<n_interval_ms, statistics_options, time_t, &statistics_options::interval_ms>,
      wfc::json::member<n_handler_map, statistics_options, std::string, &statistics_options::handler_map>,
      wfc::json::member<n_result_queue, statistics_options, std::string, &statistics_options::result_queue>
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

}}