#pragma once 

#include <wfc/inet/client/client_tcp_config.hpp>
#include <wfc/inet/client/client_configurator.hpp>
#include <string>

namespace wfc{ namespace inet{

struct client_tcp_configurator: client_configurator
{
  typedef client_tcp_config config_type;

  template<typename T>
  void operator()(T& t, const client_tcp_config& conf)
  {
    client_configurator::operator()(t, conf);
    auto cntx = t.client_context();
    cntx.async_connect = conf.async_connect;
    t.client_context(cntx);
  }
};


}}