#pragma once 
#include <string>
#include <vector>
#include <iostream>

namespace wfc{ namespace jsonrpc{

struct server_config
{
  bool enabled;
  size_t listen_threads;
  size_t worker_threads;
  std::string host;
  std::string port;

  server_config()
    : enabled(false)
    , listen_threads(0)
    , worker_threads(0)
  {
  }
};

}}
