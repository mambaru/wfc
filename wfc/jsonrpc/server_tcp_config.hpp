#pragma once 

#include <wfc/inet/server_tcp_config.hpp>

namespace wfc{ namespace jsonrpc{

struct server_tcp_config
  : inet::server_tcp_config
{
  bool close_after_response;
  server_tcp_config()
    : close_after_response(false)
  {}
};

}}
