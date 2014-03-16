#pragma once 
#include <string>

#include <wfc/inet/srv/ip/config.hpp>

namespace wfc{ namespace inet{ namespace srv{  namespace ip{ namespace tcp{

struct config
  : wfc::inet::srv::ip::config
{
  config()
    : wfc::inet::srv::ip::config()
  {
  }
};

}}}}}
