#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/aspect/call.hpp>
#include <wfc/jsonrpc/method/aspect/invoke.hpp>

#include <wfc/jsonrpc/method/aspect/invoke_stub.hpp>
#include <wfc/jsonrpc/method/aspect/ad_invoke_error.hpp>
#include <wfc/jsonrpc/method/aspect/ad_invoke_result.hpp>
#include <wfc/jsonrpc/method/aspect/request_serializer.hpp>
#include <wfc/jsonrpc/method/aspect/notify_serializer.hpp>
#include <wfc/jsonrpc/method/aspect/tags.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
struct aspect_method
  : fas::aspect< 
      // request_serializer<80u>,
      request_serializer_proxy,
      notify_serializer_proxy,
      //fas::advice<_request_serializer_, ad_request_serializer>,
      fas::advice<_invoke_error_, ad_invoke_error>,
      fas::advice<_invoke_result_, ad_invoke_result>,
      invoke_stub
    > 
{
  
};

}} // wfc


