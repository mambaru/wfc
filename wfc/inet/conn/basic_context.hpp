#pragma once

//#include <wfc/inet/srv/iactivity.hpp>

namespace wfc{ namespace inet{ namespace conn{

struct empty_context
{
};

template<typename Base = empty_context>
struct basic_context: Base
{
  bool enable_stat;
  //std::weak_ptr<iactivity> activity;
  
  basic_context()
    : Base()
    , enable_stat(false)
  {}
};

}}}
