#pragma once

#include "stat_options.hpp"
#include <wfc/json.hpp>

namespace wfc{

struct stat_options_json
{
  JSON_NAME(reduced_size)
  JSON_NAME(step_ms)
  JSON_NAME(limit)
  JSON_NAME(levels)
  JSON_NAME(prefixes)

  typedef json::object<
    stat_options,
    json::member_list<
      json::member< n_reduced_size,  ::wrtstat::aggregator_options, ::wrtstat::types::size_type, & ::wrtstat::aggregator_options::reduced_size>,
      json::member< n_step_ms, ::wrtstat::separator_options, ::wrtstat::types::time_type, &::wrtstat::separator_options::step_ts>,
      json::member< n_limit, ::wrtstat::reducer_options, ::wrtstat::types::size_type, &::wrtstat::reducer_options::limit>,
      json::member< n_levels, ::wrtstat::reducer_options, ::wrtstat::types::size_type, &::wrtstat::reducer_options::levels>,
      json::member< n_prefixes, ::wrtstat::wrtstat_options, std::vector<std::string>, &::wrtstat::wrtstat_options::prefixes, json::vector_of_strings<> >
     >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
