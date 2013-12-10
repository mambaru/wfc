#pragma once

#include <string>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

class invalid_id
  : public std::exception
{
  invalid_id(int)
  {
    
  }
};

struct error
{
  int code;
  std::string message;
};

}}}}
