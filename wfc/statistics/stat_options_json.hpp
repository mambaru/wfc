#pragma once

#include "stat_options.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace statistics{

struct aggregator_options_json
{
  typedef ::wrtstat::types::time_type time_type;
  typedef ::wrtstat::types::size_type size_type;
  typedef ::wrtstat::reducer_options reducer_options;
  typedef ::wrtstat::separator_options separator_options;
  typedef ::wrtstat::aggregator_options aggregator_options;
  
  JSON_NAME(seconds)
  JSON_NAME(milliseconds)
  JSON_NAME(microseconds)
  JSON_NAME(nanoseconds)
  
  typedef json::enumerator<
    time_type,
    json::member_list<
      json::enum_value<n_nanoseconds,  time_type, 1000000000>,
      json::enum_value<n_microseconds, time_type, 1000000>,
      json::enum_value<n_milliseconds, time_type, 1000>,
      json::enum_value<n_seconds,      time_type, 1>
    >
  > resolution_json;

  JSON_NAME(reducer_limit)
  JSON_NAME(reducer_levels)
  JSON_NAME(resolution)
  JSON_NAME(soiled_start_ts)
  JSON_NAME(outgoing_reduced_size)
  JSON_NAME(aggregation_step_ts)

  typedef json::object<
    aggregator_options,
    json::member_list<
      json::member< n_reducer_limit, reducer_options, size_type, &reducer_options::reducer_limit>,
      json::member< n_reducer_levels, reducer_options, size_type, &reducer_options::reducer_levels>,
      json::member< n_resolution, separator_options, time_type, &separator_options::resolution, resolution_json>,
      json::member< n_outgoing_reduced_size,  aggregator_options, size_type, &aggregator_options::outgoing_reduced_size>,
      json::member< n_aggregation_step_ts, separator_options, time_type, &separator_options::aggregation_step_ts>,
      json::member< n_soiled_start_ts, separator_options, time_type, &separator_options::soiled_start_ts>
    >,
    json::strict_mode
  > type;

  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list;
};
  
struct stat_options_json
{
  JSON_NAME(data_pool)
  JSON_NAME(prefixes)

  typedef json::object<
    stat_options,
    json::member_list<
      json::base<aggregator_options_json>,
      json::member< n_data_pool, ::wrtstat::manager_options, size_t, &::wrtstat::manager_options::data_pool>,
      json::member< n_prefixes, ::wrtstat::wrtstat_options, std::vector<std::string>, &::wrtstat::wrtstat_options::prefixes, json::vector_of_strings<> >
     >,
     json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
