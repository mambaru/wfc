#pragma once

namespace wfc{ namespace inet{

struct empty_context
{
};

template<typename Base = empty_context>
struct connection_context: Base
{
  bool enable_stat;
  connection_context()
    : Base()
    , enable_stat(false)
  {}
};

}}
