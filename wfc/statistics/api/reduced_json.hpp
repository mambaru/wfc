#pragma once
#include <wfc/json.hpp>
#include <wrtstat/reduced_data.hpp>
  
namespace wfc { namespace statistics{

struct reduced_info_json
{
  typedef ::wrtstat::reduced_info    reduced_info;
  typedef ::wrtstat::value_type   value_type;
  typedef ::wrtstat::size_type    size_type;
  typedef ::wrtstat::time_type    time_type;

  JSON_NAME(ts)
  JSON_NAME(avg)
  JSON_NAME(count)
  JSON_NAME(lossy)
  JSON_NAME(min)
  JSON_NAME(max)


  typedef wfc::json::object<
    reduced_info,
    json::member_list<
       json::member<n_ts,    reduced_info,  time_type, &reduced_info::ts>,
       json::member<n_avg,   reduced_info,  value_type, &reduced_info::avg>,
       json::member<n_count, reduced_info,  size_type,  &reduced_info::count>,
       json::member<n_lossy, reduced_info,  size_type,  &reduced_info::lossy>,
       json::member<n_min,   reduced_info,  value_type, &reduced_info::min>,
       json::member<n_max,   reduced_info,  value_type, &reduced_info::max>
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};


struct reduced_data_json
{
  typedef ::wrtstat::reduced_data    reduced_data;
  typedef ::wrtstat::value_type   value_type;
  typedef ::wrtstat::data_type       data_type;
  JSON_NAME(data)

  typedef wfc::json::object<
    reduced_data,
    json::member_list<
       reduced_info_json::member_list,
       json::member<n_data,  reduced_data,  data_type,  &reduced_data::data, 
          json::array< std::vector< json::value<value_type> >, 128 > >
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

struct reduced_info_json_array
{
  typedef ::wrtstat::reduced_info    reduced_info;
  typedef ::wrtstat::value_type   value_type;
  typedef ::wrtstat::size_type    size_type;
  typedef ::wrtstat::data_type    data_type;
  typedef ::wrtstat::time_type    time_type;

  // Разбили для совместимости со старым api на два списка (см. в aggregated_info_json_array)
  
  typedef fas::type_list_n<
    json::member_array<reduced_info,  time_type,  &reduced_info::ts>,
    json::member_array<reduced_info,  value_type, &reduced_info::avg>,
    json::member_array<reduced_info,  size_type,  &reduced_info::count>
  >::type head_list;

  typedef fas::type_list_n<
    json::member_array<reduced_info,  value_type, &reduced_info::min>,
    json::member_array<reduced_info,  value_type, &reduced_info::max>,
    json::member_array<reduced_info,  size_type,  &reduced_info::lossy>
  >::type tail_list;
  
  typedef wfc::json::object_array<
    reduced_info,
    json::member_list<
      head_list,
      tail_list
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};



  
}}
