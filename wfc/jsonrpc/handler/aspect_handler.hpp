#pragma once

#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <wfc/jsonrpc/handler/aspect/target.hpp>
#include <wfc/jsonrpc/handler/aspect/interface_.hpp>
#include <wfc/jsonrpc/handler/aspect/provider.hpp>
#include <wfc/jsonrpc/method/aspect/request_serializer.hpp>
#include <wfc/jsonrpc/method/aspect/notify_serializer.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

struct aspect_handler: fas::aspect<
  target<fas::empty_type>, 
  interface_<fas::empty_type>,
  provider<fas::empty_type>, 
  fas::stub<_startup_>, 
  fas::stub<_shutdown_>,
  request_serializer<80>,
  notify_serializer<80>
>{};


}} // wfc


