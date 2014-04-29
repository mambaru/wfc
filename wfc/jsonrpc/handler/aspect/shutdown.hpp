#pragma once

#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop/advice.hpp>

namespace wfc{ namespace jsonrpc{


template<typename Handler>
struct shutdown:
  fas::advice<_shutdown_, Handler>
{};

}} // wfc


