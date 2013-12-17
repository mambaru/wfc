#pragma once

namespace wfc{ namespace inet{
  
struct connection_context
{
  bool enable_stat;
  connection_context()
    : enable_stat(false)
  {}
};

}}
