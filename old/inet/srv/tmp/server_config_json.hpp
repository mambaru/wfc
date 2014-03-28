#pragma once

#include "server_config.hpp"
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace inet{

struct server_config_json
{
  FAS_NAME(enabled)
  FAS_NAME(enable_stat)
  FAS_NAME(host)
  FAS_NAME(port)

  typedef json::object<
    server_config,
    fas::type_list_n<
       json::member<n_enabled,     server_config, bool,        &server_config::enabled>,
       json::member<n_enable_stat, server_config, bool,        &server_config::enable_stat>,
       json::member<n_host,        server_config, std::string, &server_config::host>,
       json::member<n_port,        server_config, std::string, &server_config::port>
    >::type
  > type;
  
  typedef type::serializer serializer; 
};

}}
