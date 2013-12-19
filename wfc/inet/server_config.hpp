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

}}
