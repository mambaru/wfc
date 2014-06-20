#pragma once

#include <vector>
#include <string>
#include <wfc/io/basic/aspect.hpp>

namespace wfc{ namespace jsonrpc{

struct options: 
  ::wfc::io::basic::options
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