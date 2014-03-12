#pragma once

#include <wfc/jsonrpc/worker_options.hpp>
#include <vector>
#include <string>

namespace wfc{ namespace jsonrpc{

struct options
  : worker_options
{
  struct service
  {
    struct queue
    {
      int count = 1;
      std::vector<std::string> methods = {"*"};
    };
    
    int threads = 0;
    std::vector<queue> queues = {queue()};
  };
  std::vector< service> services = { service() };
};

}}