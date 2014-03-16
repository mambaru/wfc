#pragma once

#include <string>
#include <wfc/inet/srv/ip/basic_context.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{

template<typename Base = fas::empty_type>
struct basic_context
  : wfc::inet::srv::ip::basic_context< Base > 
{
  basic_context()
    : wfc::inet::srv::ip::basic_context< Base >()
  {}
};

}}}}}
