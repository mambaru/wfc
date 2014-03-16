#pragma once 

#include <wfc/inet/srv/ip/config.hpp>
#include <wfc/inet/srv/configurator.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{

struct configurator
  :  wfc::inet::srv::configurator
{
  template<typename T>
  void operator()(T& t, const config& conf)
  {
    wfc::inet::srv::configurator::operator() (t, conf);
    
    auto cntx = t.server_context();
    t.server_context(cntx);
  }
};

}}}}
