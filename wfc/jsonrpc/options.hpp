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
      std::vector<std::string> methods;
    };
    
    int threads = 0;
    std::vector<strand> strands;
  };
  std::vector<worker> workers;
  
  
  static options create() 
  {
    options opt;
    worker wrk;
    worker::strand stn;
    stn.count = 1;
    stn.methods.push_back("*");
    wrk.strands.push_back(stn);
    wrk.threads = 1;
    opt.workers.push_back(wrk);
    return opt;
  }
};

}}