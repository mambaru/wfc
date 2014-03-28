#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{
  
template<typename V = fas::empty_type>
struct context: 
  ::fas::value<_context_, V>
{
};

}}
