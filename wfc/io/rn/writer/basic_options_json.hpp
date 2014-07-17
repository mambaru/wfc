#pragma once

#include <wfc/io/rn/reader/basic_options.hpp>
#include <wfc/json/json.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace writer{
  
struct basic_options_json
{
  
  typedef ::wfc::json::object<
    basic_options,
    fas::type_list_n<
    >::type
  > type;
  
  typedef type::member_list     member_list;
  typedef type::target          target;
  typedef type::serializer      serializer;

};

}}}}