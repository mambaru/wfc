#pragma once

#include <string>
#include <wfc/inet/srv/basic_context.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{

template<typename Base = fas::empty_type>
struct basic_context
  : wfc::inet::srv::basic_context< Base > 
{
  basic_context()
    : wfc::inet::srv::basic_context< Base >()
  {}
};

}}}}
