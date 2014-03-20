#pragma once

#include <wfc/jsonrpc/handler/tags.hpp>
#include <wfc/jsonrpc/handler/target.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

struct handler_aspect: fas::aspect<
  target<fas::empty_type>, 
  interface_<fas::empty_type>,
  provider<fas::empty_type>, 
  fas::stub<_startup_>, 
  fas::stub<_shutdown_>
>{};


}} // wfc


