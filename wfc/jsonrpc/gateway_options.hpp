#pragma once 

#include <string>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct gateway_options: EngineOptions
{
  typedef EngineOptions engine_options;
  std::string incoming_target;
  std::string outgoing_target;
  bool incoming_reg = false;
  bool outgoing_reg = true;
  time_t remove_outdated_ms = 0;
};

}}