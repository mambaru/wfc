#pragma once

#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
template<typename I>
struct provider
  : fas::value<_provider_, std::weak_ptr<I> >
{
};

}} // wfc


