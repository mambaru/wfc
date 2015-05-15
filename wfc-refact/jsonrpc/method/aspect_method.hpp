#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/aspect/call.hpp>
#include <wfc/jsonrpc/method/aspect/invoke.hpp>

#include <wfc/jsonrpc/method/aspect/invoke_stub.hpp>
#include <wfc/jsonrpc/method/aspect/send_error.hpp>
#include <wfc/jsonrpc/method/aspect/send_result.hpp>
#include <wfc/jsonrpc/method/aspect/request_serializer.hpp>
#include <wfc/jsonrpc/method/aspect/notify_serializer.hpp>
#include <wfc/jsonrpc/method/aspect/process_response.hpp>
#include <wfc/jsonrpc/method/aspect/tags.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
struct aspect_method
  : fas::aspect< 
      request_serializer_proxy,
      notify_serializer_proxy,
      process_response_proxy,
      send_error_proxy,
      send_result_proxy,
      invoke_stub
    > 
{
  
};

}} // wfc


