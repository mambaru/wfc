#pragma once

#include <wfc/inet/srv/server_context.hpp>

namespace wfc{ namespace inet{
  
struct server_tcp_context: server_context
{
  int listen_threads;
  int worker_threads;
 
  server_tcp_context()
    : server_context()
    , listen_threads(0)
    , worker_threads(0)
    
  {}
};


}}