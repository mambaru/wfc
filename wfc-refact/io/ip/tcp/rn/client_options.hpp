#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/types.hpp>
#include <wfc/io/ip/tcp/rn/connection_options.hpp>
#include <wfc/io/rn/writer/aspect/aspect.hpp>



namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

struct client_options
  : ::wfc::io::basic::options
{
  connection_options connection;
  std::string host = "0.0.0.0";
  std::string port = "12345";
  int threads = 1;
  // bool non_blocking_connect = false;
  time_t reconnect_timeout = 1;
};
  
}}}}}
