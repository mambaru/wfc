#pragma once 

#include <string>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct service_options: EngineOptions
{
  typedef EngineOptions engine_options;
  std::string target;
  //time_t remove_outdated_ms = 0;
};

}}