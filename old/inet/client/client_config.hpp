#pragma once 

#include <string>


namespace wfc{ namespace inet{
  
struct client_config
{
  bool enabled;
  bool enable_stat;
  std::string host;
  std::string port;
  int worker_threads;
  size_t outgoing_warning;
  size_t outgoing_limit;


  client_config()
    : enabled(true)
    , enable_stat(false)
    , worker_threads(0)
    , outgoing_warning(0)
    , outgoing_limit(0)

  {
  }
};

}}