//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <string>
#include <wfc/jsonrpc/basic_options.hpp>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct gateway_options: basic_options<EngineOptions>
{
  typedef EngineOptions engine_options;
  std::string incoming_target_name;
  std::string outgoing_target_name;
  bool incoming_reg = false;
  bool outgoing_reg = true;
  time_t remove_outdated_ms = 0;
};

}}
