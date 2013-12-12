#pragma once 
#include <string>
#include <vector>
#include <iostream>

namespace wfc{ namespace inet{

  
struct server_config
{
  bool enabled;
  std::string host;
  std::string port;

  server_config()
    : enabled(false)
  {
  }
};

}}
