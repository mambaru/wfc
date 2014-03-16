#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/ip/tcp/rn/acceptor_options.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 
  
struct server_options: acceptor_options
{
  int threads = 3;
  // int acceptors = 1;
};
  
}}}}}