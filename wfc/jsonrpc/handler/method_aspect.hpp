#pragma once

#include <wfc/jsonrpc/handler/invoke_stub.hpp>
#include <fas/aop.hpp>



namespace wfc{ namespace jsonrpc{
  
struct method_aspect
  : fas::aspect< invoke_stub> 
{
  
};

}} // wfc


