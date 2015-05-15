#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/io/ip/udp/rn/server_options.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

typedef  ::wfc::io::ip::tcp::rn::server_options tcp_options;
typedef  ::wfc::io::ip::udp::rn::server_options udp_options;
typedef ::wfc::jsonrpc::options jsonrpc_options;

struct service_config
{
  std::shared_ptr<jsonrpc_options> jsonrpc;
  std::vector<tcp_options> tcp /*= {tcp_options()}*/;
  std::vector<udp_options> udp;
};


//typedef std::vector<service_config> service_list_config;

struct service_list_config
{
  typedef std::vector<service_config> service_list_type;
  service_list_type services /*= {service_config()}*/;
};

}}}}
