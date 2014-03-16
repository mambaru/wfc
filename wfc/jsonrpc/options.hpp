#pragma once

#include <wfc/jsonrpc/worker_options.hpp>
#include <vector>
#include <string>

namespace wfc{ namespace jsonrpc{

struct options
  : worker_options
{
  struct worker
  {
    struct strand
    {
      int count = 1;
      std::vector<std::string> methods = {"*"};
    };
    
    int threads = 0;
    std::vector<strand> strands = {strand()};
  };
  std::vector<worker> workers = { worker() };
};

}}