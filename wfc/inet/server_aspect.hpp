#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/metalist.hpp>

namespace wfc{ namespace inet{
  
template<typename A>
struct server_aspect
{
  typedef fas::metalist::advice metatype;
  typedef _server_aspect_ tag;
  typedef A advice_class;
};

}}