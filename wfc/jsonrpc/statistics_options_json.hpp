#pragma once 

#include <wfc/jsonrpc/statistics_options.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace jsonrpc{


struct statistics_options_json
{
  typedef wfc::json::object<
    statistics_options,
    wfc::json::member_list<
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;
};

}}