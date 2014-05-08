#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/io/ip/tcp/rn/server_options_json.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{


struct service_config_json
{
  JSON_NAME(jsonrpc)
  JSON_NAME(tcp)
  
  typedef ::wfc::json::object<
    service_config,
    fas::type_list_n<
      ::wfc::json::member< 
          n_jsonrpc, 
          service_config, 
          std::shared_ptr<jsonrpc_options>,
          &service_config::jsonrpc,
          ::wfc::json::pointer<
            std::shared_ptr<jsonrpc_options>,
            ::wfc::jsonrpc::options_json::type
          >
      >, 
      ::wfc::json::member< 
          n_tcp, 
          service_config, 
          std::vector< ::wfc::io::ip::tcp::rn::server_options >, 
          &service_config::tcp, 
          ::wfc::json::array< ::wfc::io::ip::tcp::rn::server_options_json::type >
      >
    >::type
  > type;

  typedef type::target     target;
  typedef type::serializer serializer;

};

struct service_list_config_json
{ 
  typedef json::member_value<
      service_list_config,
      service_list_config, 
      service_list_config::service_list_type, 
      &service_list_config::services, 
      ::wfc::json::array< std::vector<service_config_json > >
  > type;
  
  typedef type::target     target;
  typedef type::serializer serializer;


  /*
  typedef ::wfc::json::array< std::vector<service_config_json::type > > service_list_config_json;
    typedef json::member_value<
      subscribe,
      subscribe, 
      std::string, 
      &subscribe::channel/
    > type;
  */
};

}}}}
