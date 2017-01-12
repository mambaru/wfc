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