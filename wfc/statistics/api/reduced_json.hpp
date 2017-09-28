#pragma once
#include <wfc/json.hpp>
#include <wrtstat/reduced_data.hpp>
  
namespace wfc { namespace btp{

struct reduced_json
{
  typedef ::wrtstat::reduced_data    reduced;
  typedef reduced::value_type   value_type;
  typedef reduced::size_type    size_type;
  typedef reduced::data_type    data_type;

  JSON_NAME(count)
  JSON_NAME(lossy)
  JSON_NAME(min)
  JSON_NAME(max)
  JSON_NAME(avg)
  JSON_NAME(data)

  typedef wfc::json::object<
    reduced,
    json::member_list<
       json::member<n_avg,   reduced,  value_type, &reduced::avg>,
       json::member<n_count, reduced,  size_type,  &reduced::count>,
       json::member<n_lossy, reduced,  size_type,  &reduced::lossy>,
       json::member<n_min,   reduced,  value_type, &reduced::min>,
       json::member<n_max,   reduced,  value_type, &reduced::max>,
       json::member<n_data,  reduced, data_type,   &reduced::data, 
          json::array< std::vector< json::value<value_type> >, 128 > >
    >
  > type;
  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer serializer;
};


}}
