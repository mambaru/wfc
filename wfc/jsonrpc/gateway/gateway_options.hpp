#pragma once 

#include <string>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct gateway_options: EngineOptions
{
  typedef EngineOptions engine_options;
  std::string incoming_target_name;
  std::string outgoing_target_name;
  bool incoming_reg = false;
  bool outgoing_reg = true;
  time_t remove_outdated_ms = 0;
};

}}