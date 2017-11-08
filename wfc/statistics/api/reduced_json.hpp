#pragma once
#include <wfc/json.hpp>
#include <wrtstat/reduced_data.hpp>
  
namespace wfc { namespace statistics{

struct reduced_json
{
  typedef ::wrtstat::reduced_info    reduced_info;
  typedef ::wrtstat::reduced_data    reduced_data;
  typedef ::wrtstat::value_type   value_type;
  typedef ::wrtstat::size_type    size_type;
  typedef ::wrtstat::data_type    data_type;

  JSON_NAME(count)
  JSON_NAME(lossy)
  JSON_NAME(min)
  JSON_NAME(max)
  JSON_NAME(avg)
  JSON_NAME(data)

  typedef wfc::json::object<
    reduced_data,
    json::member_list<
       json::member<n_avg,   reduced_info,  value_type, &reduced_info::avg>,
       json::member<n_count, reduced_info,  size_type,  &reduced_info::count>,
       json::member<n_lossy, reduced_info,  size_type,  &reduced_info::lossy>,
       json::member<n_min,   reduced_info,  value_type, &reduced_info::min>,
       json::member<n_max,   reduced_info,  value_type, &reduced_info::max>,
       json::member<n_data,  reduced_data,  data_type,  &reduced_data::data, 
          json::array< std::vector< json::value<value_type> >, 128 > >
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};


}}
