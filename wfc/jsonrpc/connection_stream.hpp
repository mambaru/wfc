#pragma once 

#include "aspect_tcp.hpp"

#include <wfc/inet/context.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

using ::wfc::inet::context;
  
template<typename ...Args>
struct connection_stream
  : inet::connection_aspect< 
      typename fas::merge_aspect< 
        fas::aspect<Args...>, 
        aspect_stream<> 
      >::type 
    >
{
};

}}
