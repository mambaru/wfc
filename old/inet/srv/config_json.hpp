#pragma once

#include <wfc/inet/srv/config.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace inet{ namespace srv{

struct config_json
{
  FAS_NAME(enabled)
  FAS_NAME(enable_stat)
  FAS_NAME(host)
  FAS_NAME(port)

  typedef json::object<
    config,
    fas::type_list_n<
       json::member<n_enabled,     config, bool,        &config::enabled>,
       json::member<n_enable_stat, config, bool,        &config::enable_stat>,
       json::member<n_host,        config, std::string, &config::host>,
       json::member<n_port,        config, std::string, &config::port>
    >::type
  > type;
  
  typedef type::serializer serializer; 
};

}}}
