#pragma once

#include <wfc/inet/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{
  
/*

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


*/

template< template<typename> class T >
struct basic_context_class:
  fas::provider_t<_basic_context_, T>
{
};

template<typename V>
struct context: 
  ::fas::value<_context_, V>
{
};



}}
