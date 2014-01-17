#pragma once

#include <wfc/inet/connection_class.hpp>
#include <wfc/inet/connection_base_class.hpp>
#include <wfc/inet/connection_aspect.hpp>
#include <wfc/inet/server_aspect.hpp>

#include <wfc/inet/echo/aspect_stream_rn.hpp>
#include <wfc/inet/srv/aspect_server_tcp.hpp>
#include <wfc/inet/conn/dgram_connection.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{

struct aspect_default: fas::aspect< fas::type_list_n<
  connection_aspect< echo::aspect_stream_rn >,
  server_aspect< aspect_server_tcp >,
  connection_base_class< fas::aspect_class >,
  connection_class< dgram_connection >
>::type> 
{
};

}}