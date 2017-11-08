#pragma once
#include <wfc/json.hpp>
#include <wrtstat/aggregated_data.hpp>
#include <wfc/statistics/api/reduced_json.hpp>  

namespace wfc { namespace statistics{

struct aggregated_json
{
  typedef ::wrtstat::aggregated_data aggregated_data;
  typedef ::wrtstat::aggregated_perc aggregated_perc;
  typedef ::wrtstat::value_type     value_type;

  JSON_NAME(perc50)
  JSON_NAME(perc80)
  JSON_NAME(perc95)
  JSON_NAME(perc99)
  JSON_NAME(perc100)

  typedef wfc::json::object<
    aggregated_data,
    json::member_list<
       json::member<n_perc50,   aggregated_perc,   value_type,  &aggregated_perc::perc50>,
       json::member<n_perc80,   aggregated_perc,   value_type,  &aggregated_perc::perc80>,
       json::member<n_perc95,   aggregated_perc,   value_type,  &aggregated_perc::perc95>,
       json::member<n_perc99,   aggregated_perc,   value_type,  &aggregated_perc::perc99>,
       json::member<n_perc100,  aggregated_perc,   value_type,  &aggregated_perc::perc100>,
       json::base<reduced_json>
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

}}
