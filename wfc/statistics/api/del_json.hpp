//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/json.hpp>
#include <wfc/statistics/api/del.hpp>
#include "aggregated_json.hpp"

namespace wfc { namespace statistics {

namespace request {
  
  struct del_json
  {
    JSON_NAME(name)
 
    typedef wfc::json::object<
      del,
      json::member_list<
        json::member<n_name, del, std::string, &del::name>
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct del_json
  {
    FAS_NAME(status)
    
    typedef wfc::json::object<
      del,
      fas::type_list_n<
        wfc::json::member<n_status, del, bool, &del::status>
      >::type
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}}
