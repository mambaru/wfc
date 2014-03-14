#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/io/ip/tcp/rn/server_options.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

typedef  ::wfc::io::ip::tcp::rn::server_options tcp_options;

struct configuration
{
   ::wfc::jsonrpc::options jsonrpc;
  std::vector<tcp_options> tcp = {tcp_options()};
};


}}}}
