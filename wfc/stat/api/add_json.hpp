#pragma once

#include <wfc/json.hpp>
#include <wfc/stat/api/add.hpp>
#include "aggregated_json.hpp"

namespace wfc { namespace btp {


namespace request {
  
  struct add_json
  {
    typedef ::wrtstat::aggregated_data aggregated;
    typedef aggregated::value_type     value_type;
    typedef aggregated::size_type      size_type;
    typedef wrtstat::types::time_type   time_type;
    typedef wrtstat::types::data_type   data_type;
    typedef wrtstat::types::data_ptr   data_ptr;
    typedef json::array< std::vector< json::value<value_type> > > data_json;

    JSON_NAME(ts)
    JSON_NAME(name)
    JSON_NAME(ag)
    JSON_NAME(cl)
 
    typedef wfc::json::object<
      add,
      json::member_list<
        json::member<n_ts,   add, time_type, &add::ts>,
        json::member<n_name, add, std::string, &add::name>,
        json::member<n_ag,   add, aggregated, &add::ag, aggregated_json>,
        json::member<n_cl,   add, data_type, &add::cl, data_json >
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct add_json
  {
    FAS_NAME(result)
    
    typedef wfc::json::object<
      add,
      fas::type_list_n<
        wfc::json::member<n_result, add, bool, &add::result>
      >::type
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}}
