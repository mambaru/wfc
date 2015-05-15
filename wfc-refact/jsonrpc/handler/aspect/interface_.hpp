#pragma once

#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop/metalist.hpp>

namespace wfc{ namespace jsonrpc{
  
template<typename I>
struct interface_ 
{
  typedef fas::metalist::advice metatype;
  typedef _interface_ tag;
  typedef I advice_class;
};

}} // wfc


