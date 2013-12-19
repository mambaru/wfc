#pragma once

#include <string>

namespace wfc{ namespace inet{
  
struct server_context
{
  bool enable_stat;
  std::string host;
  std::string port;
 
  server_context()
    : enable_stat(false)
  {}
};


}}