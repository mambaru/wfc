#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{
  
template< template<typename> class T >
struct basic_context_class:
  fas::provider_t<_basic_context_, T>
{
};


}}
