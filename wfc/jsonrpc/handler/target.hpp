#pragma once

#include <wfc/jsonrpc/handler/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename I>
struct target
  : fas::value<_target_, std::weak_ptr<I> >
{
};

}} 


