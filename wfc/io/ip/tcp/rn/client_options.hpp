#pragma once

#include <cstddef>
#include <functional>
#include <wfc/io/types.hpp>
#include <wfc/io/ip/tcp/rn/connection_options.hpp>
#include <wfc/io/rn/writer/aspect.hpp>



namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

// Вынести основной в io::client  
struct client_options
  : ::wfc::io::basic::options
  /*, ::wfc::io::rn::reader::basic_options
  , ::wfc::io::rn::writer::basic_options
  */
{
  // bool keep_alive = false;connection
  connection_options connection;
};
  
}}}}}
