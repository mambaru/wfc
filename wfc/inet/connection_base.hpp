#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop/type.hpp>
#include <fas/aop/aspect_class.hpp>

namespace wfc{ namespace inet{
  
template<template<typename> class ConnBase= ::fas::aspect_class >
struct connection_base_t
{
  template<typename A>
  using type = ConnBase<A>;
};


template<template<typename> class ConnBase= ::fas::aspect_class >
struct connection_base
  : fas::type<_connection_base_, connection_base_t<ConnBase> >
{};

}}
