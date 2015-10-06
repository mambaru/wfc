#pragma once 

#include <string>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct options: EngineOptions
{
  typedef EngineOptions engine_options;
  std::string target;
};

}}