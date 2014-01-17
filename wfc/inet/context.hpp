#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/value.hpp>

namespace wfc{ namespace inet{
  

template< template<typename> class T >
struct basic_context_t:
  fas::provider_t<_basic_context_, T>
{
};

template<typename V>
struct context: 
  ::fas::value<_context_, V>
{
};



}}
