#pragma once
#include <wfc/json.hpp>
#include <wrtstat/aggregated_data.hpp>

/*

reduced_data
  // Общее количество
  size_t count;
  // Количество не учтенных
  size_t lossy;
  // min - это не 0% с потярми
  value_type min;
  // Ахтунг! Если есть потери, то max-это не 100%
  value_type max;
  // Среднее считаем здесь, для точности
  value_type avg;
  data_type data;

aggregated_data
  value_type perc50;
  value_type perc80;
  value_type perc95;
  value_type perc99;
  value_type perc100;

*/
  
namespace wfc { namespace btp{

struct aggregated_json
{
  typedef ::wrtstat::reduced_data    reduced_data;
  typedef ::wrtstat::aggregated_data aggregated;
  typedef aggregated::value_type     value_type;
  typedef aggregated::size_type      size_type;

  
  
  JSON_NAME(count)
  JSON_NAME(lossy)
  JSON_NAME(min)
  JSON_NAME(perc50)
  JSON_NAME(perc80)
  JSON_NAME(perc95)
  JSON_NAME(perc99)
  JSON_NAME(perc100)
  JSON_NAME(max)
  JSON_NAME(avg)

  typedef wfc::json::object<
    aggregated,
    json::member_list<
       json::member<n_avg,      reduced_data, value_type,  &reduced_data::avg>,
       json::member<n_count,    reduced_data, size_type,   &reduced_data::count>,
       json::member<n_lossy,    reduced_data, size_type,   &reduced_data::lossy>,
       json::member<n_min,      reduced_data,   value_type,  &reduced_data::min>,
       json::member<n_max,      reduced_data,   value_type,  &reduced_data::max>,
       json::member<n_perc50,   aggregated,   value_type,  &aggregated::perc50>,
       json::member<n_perc80,   aggregated,   value_type,  &aggregated::perc80>,
       json::member<n_perc95,   aggregated,   value_type,  &aggregated::perc95>,
       json::member<n_perc99,   aggregated,   value_type,  &aggregated::perc99>,
       json::member<n_perc100,  aggregated,   value_type,  &aggregated::perc100>
       
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};

}}
