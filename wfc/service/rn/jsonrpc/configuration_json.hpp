#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/options_json.hpp>
#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{


struct configuration_json
{
  JSON_NAME(jsonrpc)
  
  typedef ::wfc::json::object<
    configuration,
    fas::type_list_n<
      ::wfc::json::member< n_jsonrpc, configuration, ::wfc::jsonrpc::options, &configuration::jsonrpc, ::wfc::jsonrpc::options_json::type >
    >::type
  > type;
};

}}}}
