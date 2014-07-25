#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/ip/tcp/rn/acceptor_options.hpp>
#include <wfc/io/server/server.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 
  
struct server_options
  : ::wfc::io::server::options< acceptor_options >
{
  // int threads = 3;
  // int acceptors = 1;
};
  
}}}}}