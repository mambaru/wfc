#pragma once


namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct request
{
  int id;
  std::string method;
  
  request(): id(-1)
  {
    method.reserve(64)  
  }
};

}}}}

#endif
