#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/io/ip/tcp/rn/client_options.hpp>
#include <wfc/io/ip/tcp/rn/client_options_json.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{


struct gateway_config_json
{
  JSON_NAME(jsonrpc)
  JSON_NAME(tcp)
  
  typedef ::wfc::json::object<
    gateway_config,
    fas::type_list_n<
      ::wfc::json::member< 
          n_jsonrpc, 
          gateway_config, 
          ::wfc::jsonrpc::options,
          &gateway_config::jsonrpc, 
          ::wfc::jsonrpc::options_json::type 
      >, 
      ::wfc::json::member< 
          n_tcp, 
          gateway_config, 
          std::vector< ::wfc::io::ip::tcp::rn::client_options >, 
          &gateway_config::tcp, 
          ::wfc::json::array< ::wfc::io::ip::tcp::rn::client_options_json::type >
      >
    >::type
  > type;
};

struct gateway_list_config_json
{ 
  typedef json::member_value<
      gateway_list_config,
      gateway_list_config, 
      gateway_list_config::gateway_list_type, 
      &gateway_list_config::gateways, 
      ::wfc::json::array< std::vector<gateway_config_json::type > >
  > type;

  /*
  typedef ::wfc::json::array< std::vector<gateway_config_json::type > > gateway_list_config_json;
    typedef json::member_value<
      subscribe,
      subscribe, 
      std::string, 
      &subscribe::channel/
    > type;
  */
};

}}}}
