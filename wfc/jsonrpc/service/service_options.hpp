//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct service_options: EngineOptions
{
  typedef EngineOptions engine_options;
  std::string target_name;
  time_t remove_outdated_ms = 0;
  bool allow_non_jsonrpc = false;

  // TODO: сделать саму конфигурацию
  static std::vector<std::string> get_schema_list() { return {"example"}; }
};

}}
