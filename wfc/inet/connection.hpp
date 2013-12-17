#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/type.hpp>

namespace wfc{ namespace inet{
  
template<template<typename, template<typename> class> class Conn>
struct connection_t
{
  template<typename A, template<typename> class AspectClass >
  using type = Conn<A, AspectClass>;
};

template< template<typename, template<typename> class> class Conn>
struct connection  
  : fas::type<_connection_, connection_t<Conn> >
{};

}}