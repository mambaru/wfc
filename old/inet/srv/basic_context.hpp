#pragma once

#include <string>

namespace wfc{ namespace inet{ namespace srv{
  
template<typename Base = fas::empty_type>
struct basic_context: Base
{
  bool enable_stat;
  std::string host;
  std::string port;
  
  basic_context()
    : Base()
    , enable_stat(false)
  {}
  
};


}}}