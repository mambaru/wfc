#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/metalist.hpp>
#include <fas/aop/aspect.hpp>

namespace wfc{ namespace inet{
  
template<typename A>
struct connection_aspect
{
  typedef fas::metalist::advice metatype;
  typedef _connection_aspect_ tag;
  typedef A advice_class;
};

}}