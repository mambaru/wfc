#pragma once

#include <wfc/jsonrpc/method/aspect/tags.hpp>
#include <wfc/jsonrpc/method/aspect/name.hpp>
#include <wfc/jsonrpc/method/aspect/call.hpp>
#include <wfc/jsonrpc/method/aspect/invoke.hpp>
#include <wfc/jsonrpc/method/aspect/invoke_stub.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
struct aspect_method
  : fas::aspect< invoke_stub> 
{
  
};

}} // wfc


