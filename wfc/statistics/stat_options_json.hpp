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

  JSON_NAME(reduced_size)
  JSON_NAME(step_ts)
  JSON_NAME(limit)
  JSON_NAME(levels)
  JSON_NAME(resolution)
  JSON_NAME(soiled_start)

  typedef json::object<
    aggregator_options,
    json::member_list<
      json::member< n_reduced_size,  aggregator_options, size_type, &aggregator_options::reduced_size>,
      json::member< n_step_ts, separator_options, time_type, &separator_options::step_ts>,
      json::member< n_resolution, separator_options, time_type, &separator_options::resolution, resolution_json>,
      json::member< n_soiled_start, separator_options, time_type, &separator_options::soiled_start>,
      json::member< n_limit, reducer_options, size_type, &reducer_options::limit>,
      json::member< n_levels, reducer_options, size_type, &reducer_options::levels>
    >,
    json::strict_mode
  > type;

  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list;
};
  
struct stat_options_json
{
  JSON_NAME(pool)
  JSON_NAME(prefixes)

  typedef json::object<
    stat_options,
    json::member_list<
      json::base<aggregator_options_json>,
      json::member< n_pool, ::wrtstat::manager_options, size_t, &::wrtstat::manager_options::pool>,
      json::member< n_prefixes, ::wrtstat::wrtstat_options, std::vector<std::string>, &::wrtstat::wrtstat_options::prefixes, json::vector_of_strings<> >
     >,
     json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
