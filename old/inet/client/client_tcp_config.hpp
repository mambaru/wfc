#pragma once 

#include <wfc/inet/client/client_config.hpp>
#include <string>

namespace wfc{ namespace inet{
  
struct client_tcp_config: client_config
{
  bool async_connect;
  client_tcp_config()
    : client_config()
    , async_connect(false)
  {
  }
};

}}