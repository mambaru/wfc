#pragma once
#include <wfc/gateway/client_options.hpp>
#include <wfc/io/ip/udp/rn/client_options.hpp>

namespace wfc{ namespace gateway{ 

struct udp_options: client_options< ::wfc::io::ip::udp::rn::client_options > {};

}}
