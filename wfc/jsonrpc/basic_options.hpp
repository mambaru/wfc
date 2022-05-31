//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2022
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <wflow/owner.hpp>
#include <wfc/iinterface.hpp>

namespace wfc{ namespace jsonrpc{

template<typename EngineOptions>
struct basic_options: EngineOptions
{
  // не сериализуемые опции, например  для ad_callback
  std::string domain_name;
  iow::io::io_id_t tracking_id = iinterface::bad_id;
  bool disable_tracking_log = false;
  wflow::owner* powner = nullptr;
  
};

}}

