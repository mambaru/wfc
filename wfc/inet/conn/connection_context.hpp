#pragma once

#include <wfc/inet/srv/iactivity.hpp>

namespace wfc{ namespace inet{

struct empty_context
{
};

template<typename Base = empty_context>
struct connection_context: Base
{
  bool enable_stat;
  std::weak_ptr<iactivity> activity;
  
  connection_context()
    : Base()
    , enable_stat(false)
  {}
};

}}
