#pragma once

namespace wfc{ namespace inet{ namespace conn{

struct context
{
  bool enable_stat;
  
  context()
    : enable_stat(false)
  {}
};

}}}
