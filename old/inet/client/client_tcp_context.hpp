#pragma once 

#include <wfc/inet/client/client_context.hpp>
#include <string>

namespace wfc{ namespace inet{
  
struct client_tcp_context: client_context
{
  int worker_threads;
  bool async_connect;
  client_tcp_context()
    : client_context()
    , worker_threads(0)
    , async_connect(false)
  {}
};

}}