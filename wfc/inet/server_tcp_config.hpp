#pragma once 
#include <wfc/inet/server_config.hpp>


namespace wfc{ namespace inet{

struct server_tcp_config: server_config
{
  int listen_threads;
  int worker_threads;
  
  size_t outgoing_warning;
  size_t outgoing_limit;


  server_tcp_config()
    : server_config()
    , listen_threads(0)
    , worker_threads(0)
    , outgoing_warning(0)
    , outgoing_limit(0)
  {
  }
};

}}
