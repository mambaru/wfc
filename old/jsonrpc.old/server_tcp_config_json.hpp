#pragma once

#include <wfc/inet/srv/tmp/server_tcp_config_json.hpp>
#include <wfc/jsonrpc/server_tcp_config.hpp>

namespace wfc{ namespace jsonrpc{

struct server_tcp_config_json
{
  FAS_NAME(close_after_response)
  
  typedef json::object<
    server_tcp_config,
    fas::type_list_n<
      json::base< inet::server_tcp_config_json::type>,
      json::member<n_close_after_response, server_tcp_config, bool, &server_tcp_config::close_after_response>
    >::type
  > type;
  
  typedef type::serializer serializer; 
};

}}
