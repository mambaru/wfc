#pragma once 

#include <wfc/inet/srv/config.hpp>

namespace wfc{ namespace inet{ namespace srv{

struct configurator
{
  // typedef server_config config_type;
  template<typename T>
  void operator()(T& t, const config& conf)
  {
    auto cntx = t.server_context();
    cntx.enable_stat = conf.enable_stat;
    cntx.host = conf.host;
    cntx.port = conf.port;
    t.server_context(cntx);
  }
};

}}}
