#pragma once

#include <wfc/jsonrpc/handler/aspect/tags.hpp>
#include <wfc/jsonrpc/handler/aspect/target.hpp>
#include <wfc/jsonrpc/handler/aspect/interface_.hpp>
#include <wfc/jsonrpc/handler/aspect/dual_interface.hpp>
#include <wfc/jsonrpc/handler/aspect/provider.hpp>
#include <wfc/jsonrpc/handler/aspect/startup.hpp>
#include <wfc/jsonrpc/handler/aspect/shutdown.hpp>
#include <wfc/jsonrpc/handler/aspect/context.hpp>
#include <wfc/jsonrpc/handler/aspect/ad_response_handler.hpp>
#include <wfc/jsonrpc/handler/aspect/ad_send_request.hpp>
#include <wfc/jsonrpc/handler/aspect/ad_send_notify.hpp>
#include <wfc/jsonrpc/handler/ihandler.hpp>

#include <wfc/jsonrpc/method/aspect/request_serializer.hpp>
#include <wfc/jsonrpc/method/aspect/notify_serializer.hpp>
#include <wfc/jsonrpc/method/aspect/process_response.hpp>
#include <wfc/jsonrpc/method/aspect/send_error.hpp>
#include <wfc/jsonrpc/method/aspect/send_result.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

struct aspect_handler: fas::aspect<
  target<fas::empty_type>, 
  interface_<fas::empty_type>,
  provider<fas::empty_type>, 
  fas::stub<_startup_>, 
  fas::stub<_shutdown_>,
  request_serializer<80>,
  notify_serializer<80>,
  send_error<80>,
  send_result<80>,
  process_response,
  context<fas::empty_type>,
  fas::advice<_response_handler_, ad_response_handler>,
  fas::type<_ihandler_, ihandler>,
  fas::advice<_send_request_, ad_send_request>,
  fas::advice<_send_notify_, ad_send_notify>
>{};


}} // wfc


