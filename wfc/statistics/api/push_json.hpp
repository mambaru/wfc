#pragma once

#include <wfc/json.hpp>
#include <wfc/statistics/api/push.hpp>
#include "aggregated_json.hpp"

namespace wfc { namespace statistics {


namespace request {
  
  struct push_json
  {
    JSON_NAME(name)
 
    typedef wfc::json::object<
      push,
      json::member_list<
        json::base<aggregated_json>,
        json::member<n_name, push, std::string, &push::name>
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct push_json
  {
    FAS_NAME(result)
    
    typedef wfc::json::object<
      push,
      fas::type_list_n<
        wfc::json::member<n_result, push, bool, &push::result>
      >::type
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}}
