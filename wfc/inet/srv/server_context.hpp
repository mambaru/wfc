#pragma once

#include <string>

namespace wfc{ namespace inet{
  
struct server_context
{
  std::string host;
  std::string port;
  bool enable_stat;
  int listen_threads;
  int worker_threads;
 
  server_context()
    : enable_stat(false)
    , listen_threads(0)
    , worker_threads(0)
    
  {}
};


}}