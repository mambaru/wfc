#pragma once

#include <wfc/inet/server_tcp_config_json.hpp>
#include <wfc/jsonrpc/server_tcp_config.hpp>

namespace wfc{ namespace jsonrpc{

struct server_tcp_config_json
{
  typedef json::object<
    server_tcp_config,
    fas::type_list_n<
       json::base< inet::server_tcp_config_json::type>
    >::type
  > type;
  
  typedef type::serializer serializer; 
};

}}
