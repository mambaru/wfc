#pragma once
#include <wfc/gateway/tcp_options.hpp>
#include <wfc/io/ip/tcp/rn/client_options.hpp>

namespace wfc{ namespace gateway{ 

struct gateway_config
{
  bool enabled = false;
  std::vector<tcp_options> tcp;
};

}}
