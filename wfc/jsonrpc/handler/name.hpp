#pragma once

#include <wfc/jsonrpc/handler/tags.hpp>
#include <fas/aop.hpp>
#include <fas/functional/tstring.hpp>

#define JSONRPC_TAG(x) FAS_STRING( _##x##_, #x)
#define JSONRPC_TAG2(x,y) FAS_STRING( x, y)

namespace wfc{ namespace jsonrpc{
  
template<typename N>
struct name
{
  typedef fas::metalist::advice metatype;
  typedef _name_ tag;
  typedef name<N> advice_class;
  typedef N name_type;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  const char* operator()() const
  {
    return name_type()();
  }
};

}} // wfc


