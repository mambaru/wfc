#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/value.hpp>

namespace wfc{ namespace inet{
  
/*
template<typename V>
struct context: 
  ::fas::value<_context_, V>
{
};
*/

template< template<typename> class T >
struct basic_context_t:
  fas::provider_t<_basic_context_, T>
{
  /*
  typedef metalist::advice metatype;
  typedef _basic_context_ tag;
  typedef basic_context_t<Tg, P> advice_class;

  template<typename T>
  struct apply
  {
    typedef P< typename T::aspect::template advice_cast<>::type > type;
  };
  */
};

template<typename V>
struct context: 
  ::fas::value<_context_, V>
{
};



}}
