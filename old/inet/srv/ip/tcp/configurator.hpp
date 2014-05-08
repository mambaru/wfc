#pragma once 

#include <wfc/inet/srv/ip/tcp/config.hpp>
#include <wfc/inet/srv/ip/configurator.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{

struct configurator
  :  wfc::inet::srv::ip::configurator
{
  typedef config config_type;
  template<typename T>
  void operator()(T& t, const config& conf)
  {
    wfc::inet::srv::ip::configurator::operator()(t, conf);
    
    auto cntx = t.server_context();
    t.server_context(cntx);
  }
};

}}}}}
