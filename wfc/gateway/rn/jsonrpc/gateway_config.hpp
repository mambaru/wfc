#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/io/ip/tcp/rn/client_options.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>
#include <wfc/pubsub/pubsub_gateway.hpp>

namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{

typedef  ::wfc::io::ip::tcp::rn::client_options tcp_options;
typedef  ::wfc::pubsub::pubsub_gateway_options  pubsub_options;
typedef  ::wfc::jsonrpc::options jsonrpc_options;

struct gateway_config
{
  std::shared_ptr<jsonrpc_options> jsonrpc;
  std::vector<tcp_options> tcp /*= {tcp_options()}*/;
  std::vector<pubsub_options> pubsub /*= {pubsub_options()}*/;
};

struct gateway_list_config
{
  typedef std::vector<gateway_config> gateway_list_type;
  gateway_list_type gateways /*= {gateway_config()}*/;
};

}}}}
