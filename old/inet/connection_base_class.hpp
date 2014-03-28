#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/type.hpp>
#include <fas/aop/aspect_class.hpp>

namespace wfc{ namespace inet{
  
template<template<typename> class ConnBase= ::fas::aspect_class >
struct connection_base_class_t
{
  template<typename A>
  using type = ConnBase<A>;
};


template<template<typename> class ConnBase= ::fas::aspect_class >
struct connection_base_class
  : fas::type<_connection_base_class_, connection_base_class_t<ConnBase> >
{};

}}
