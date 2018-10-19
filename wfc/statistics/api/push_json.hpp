//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

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
        json::member<n_name, push, std::string, &push::name>,
        json::base<aggregated_data_json>
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
    FAS_NAME(status)
    
    typedef wfc::json::object<
      push,
      fas::type_list_n<
        wfc::json::member<n_status, push, bool, &push::status>
      >::type
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}}
