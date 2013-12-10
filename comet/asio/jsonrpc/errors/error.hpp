#pragma once

#include <string>

namespace mamba{ namespace comet{ namespace inet{ namespace jsonrpc{

struct error
{
  int code;
  std::string message;
};

}}}}
