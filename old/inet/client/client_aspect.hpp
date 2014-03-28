#pragma once 
#include <fas/aop/metalist.hpp>
#include <wfc/inet/client/tags.hpp>


namespace wfc{ namespace inet{
  
template<typename A>
struct client_aspect
{
  typedef fas::metalist::advice metatype;
  typedef _client_aspect_ tag;
  typedef A advice_class;
};

}}