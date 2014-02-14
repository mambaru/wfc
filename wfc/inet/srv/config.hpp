#pragma once 
#include <string>

namespace wfc{ namespace inet{ namespace srv{

struct config
{
  bool enabled;
  bool enable_stat;
  std::string host;
  std::string port;

  config()
    : enabled(true)
    , enable_stat(false)
  {
  }
};

}}}
