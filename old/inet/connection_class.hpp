#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/type.hpp>

namespace wfc{ namespace inet{
  
template<template<typename, template<typename> class> class Conn>
struct connection_class_t
{
  template<typename A, template<typename> class AspectClass >
  using type = Conn<A, AspectClass>;
};

template< template<typename, template<typename> class> class Conn>
struct connection_class
  : fas::type<_connection_class_, connection_class_t<Conn> >
{};

}}