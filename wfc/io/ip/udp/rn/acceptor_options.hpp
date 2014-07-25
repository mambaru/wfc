#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/ip/udp/rn/connection_options.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{ 
  
struct acceptor_options
  : ::wfc::io::basic::options
{
  connection_options connection;
  std::string host /*= "0.0.0.0"*/;
  std::string port /*= "12345"*/;
};
  
}}}}}