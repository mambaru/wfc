#pragma once

#include <wfc/json.hpp>
#include <wfc/statistics/api/add.hpp>
#include "aggregated_json.hpp"

namespace wfc { namespace btp {


namespace request {
  
  struct add_json
  {
    JSON_NAME(name)
 
    typedef wfc::json::object<
      add,
      json::member_list<
        json::base<aggregated_json>,
        json::member<n_name, add, std::string, &add::name>
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
