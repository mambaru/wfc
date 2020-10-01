//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/json.hpp>
#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <wrtstat/aggregator/api/aggregated_info.hpp>
#include <wfc/statistics/api/reduced_json.hpp>  

namespace wfc { namespace statistics{
  
struct aggregated_perc_json
{
  typedef ::wrtstat::aggregated_perc aggregated_perc;
  typedef ::wrtstat::value_type     value_type;

  JSON_NAME(perc50)
  JSON_NAME(perc80)
  JSON_NAME(perc95)
  JSON_NAME(perc99)
  JSON_NAME(perc100)

  typedef wfc::json::object<
    aggregated_perc,
    json::member_list<
       json::member<n_perc50,   aggregated_perc,   value_type,  &aggregated_perc::perc50>,
       json::member<n_perc80,   aggregated_perc,   value_type,  &aggregated_perc::perc80>,
       json::member<n_perc95,   aggregated_perc,   value_type,  &aggregated_perc::perc95>,
       json::member<n_perc99,   aggregated_perc,   value_type,  &aggregated_perc::perc99>,
       json::member<n_perc100,  aggregated_perc,   value_type,  &aggregated_perc::perc100>
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

struct aggregated_perc_json_array
{
  typedef ::wrtstat::aggregated_perc aggregated_perc;
  typedef ::wrtstat::value_type     value_type;

  JSON_NAME(perc50)
  JSON_NAME(perc80)
  JSON_NAME(perc95)
  JSON_NAME(perc99)
  JSON_NAME(perc100)

  typedef wfc::json::object_array<
    aggregated_perc,
    json::member_list<
       json::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc50>,
       json::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc80>,
       json::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc95>,
       json::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc99>,
       json::member_array<aggregated_perc,   value_type,  &aggregated_perc::perc100>
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

struct aggregated_data_json
{
  typedef ::wrtstat::aggregated_data aggregated_data;
  typedef wfc::json::object<
    aggregated_data,
    json::member_list<
       json::base<aggregated_perc_json>,
       json::base<reduced_data_json>
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

struct aggregated_info_json
{
  typedef ::wrtstat::aggregated_info aggregated_info;
  typedef wfc::json::object<
    aggregated_info,
    json::member_list<
       json::base<aggregated_perc_json>,
       json::base<reduced_info_json>
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

struct aggregated_info_json_array
{
  typedef ::wrtstat::aggregated_info aggregated_info;
  typedef wfc::json::object_array<
    aggregated_info,
    json::member_list<
      reduced_info_json_array::head_list,
      aggregated_perc_json_array::member_list,
      reduced_info_json_array::tail_list
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

}}
