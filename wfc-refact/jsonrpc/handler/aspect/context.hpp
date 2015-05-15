#pragma once

#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename T>
struct context
  : fas::value<_context_, T >
{
};

}} // wfc


