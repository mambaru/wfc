#pragma once

#include <wfc/inet/srv/ip/tcp/config.hpp
#include <wfc/inet/srv/ip/config_json.hpp>

#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{

struct config_json
{
  typedef json::object<
    config,
    fas::type_list_n<
      json::base< wfc::inet::srv::ip::config_json::type >
    >::type
  > type;
  
  typedef type::serializer serializer; 
};

}}}}}
