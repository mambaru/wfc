#pragma once 

#include <string>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct gateway_options: EngineOptions
{
  typedef EngineOptions engine_options;
  std::string incoming_target;
  std::string outgoing_target;
};

}}