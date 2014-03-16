#pragma once 
#include <string>
#include <vector>
#include <iostream>

namespace wfc{ namespace inet{

struct server_config
{
  bool enabled;
  bool enable_stat;
  std::string host;
  std::string port;

  server_config()
    : enabled(true)
    , enable_stat(false)
  {
  }
};

struct server_configurator
{
  typedef server_config config_type;

  template<typename T>
  void operator()(T& t, const server_config& conf)
  {
    auto cntx = t.server_context();
    cntx.enable_stat = conf.enable_stat;
    cntx.host = conf.host;
    cntx.port = conf.port;
    t.server_context(cntx);
  }
};

}}
