//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <string>
#include <ctime>
namespace wfc{ namespace jsonrpc{

struct statistics_options
{
  time_t interval_ms = 1000;
  std::string handler_map  = ".handler_map";
  std::string result_map   = ".result_map";
  std::string result_queue = ".result_queue ";
};

}}
