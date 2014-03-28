#pragma once 

#include <wfc/inet/connection_class.hpp>
#include <wfc/inet/connection_base_class.hpp>
#include <wfc/inet/connection_aspect.hpp>
#include <wfc/inet/client/client_aspect.hpp>

#include <wfc/inet/conn/echo/rn/stream/tcp/aspect.hpp>
#include <wfc/inet/client/aspect_client_tcp.hpp>
#include <wfc/inet/connection.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace inet{
  
struct aspect_client_default
: fas::aspect< 
    connection_aspect< conn::echo::rn::stream::tcp::aspect/*conn::echo::aspect_stream_rn*/ >,
    client_aspect< aspect_client_tcp >,
    connection_base_class< fas::aspect_class >,
    connection_class< connection >
  > 
{
};

}}