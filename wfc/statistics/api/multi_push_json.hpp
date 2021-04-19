//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/statistics/api/push_json.hpp>
#include <wfc/statistics/api/multi_push.hpp>
#include <wfc/json.hpp>

namespace wfc { namespace statistics {

namespace request {
  
  struct multi_push_json
  {
    JSON_NAME(data)
 
    typedef wfc::json::object<
      multi_push,
      json::member_list<
        json::member<n_data, multi_push, std::deque<push>, &multi_push::data, json::deque_of<push_json> >
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct multi_push_json
  {
    JSON_NAME(status)
    
    typedef json::object<
      multi_push,
      json::member_list<
        json::member<n_status, multi_push, bool, &multi_push::status>
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

}}
