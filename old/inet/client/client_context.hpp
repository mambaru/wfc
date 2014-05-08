#pragma once 

#include <string>


namespace wfc{ namespace inet{
  
struct client_context
{
  bool enable_stat;
  std::string host;
  std::string port;
 
  client_context()
    : enable_stat(false)
  {}
};

}}