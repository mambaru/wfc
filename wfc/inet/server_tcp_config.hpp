#pragma once 
#include <wfc/inet/server_config.hpp>


namespace wfc{ namespace inet{

struct server_tcp_config: server_config
{
  int listen_threads;
  int worker_threads;

  server_tcp_config()
    : server_config()
    , listen_threads(0)
    , worker_threads(0)
  {
  }
};

}}
