#pragma once

#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop/advice.hpp>

namespace wfc{ namespace jsonrpc{


template<typename Handler>
struct startup:
  fas::advice<_startup_, Handler>
{};


}} // wfc


