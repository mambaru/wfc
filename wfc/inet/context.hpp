#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/value.hpp>

namespace wfc{ namespace inet{
  
template<typename V>
struct context: 
  ::fas::value<_context_, V>
{
};

}}
