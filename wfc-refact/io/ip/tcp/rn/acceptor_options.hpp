#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/ip/tcp/rn/connection_options.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 
  
struct acceptor_options
  : ::wfc::io::basic::options
{
  size_t backlog = 1024;
  connection_options connection;
  std::string host /*= "0.0.0.0"*/;
  std::string port /*= "12345"*/;
};
  
}}}}}