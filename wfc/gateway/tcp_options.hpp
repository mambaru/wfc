#pragma once
#include <wfc/gateway/client_options.hpp>
#include <wfc/io/ip/tcp/rn/client_options.hpp>

namespace wfc{ namespace gateway{ 

struct tcp_options: client_options< ::wfc::io::ip::tcp::rn::client_options > {};

}}
