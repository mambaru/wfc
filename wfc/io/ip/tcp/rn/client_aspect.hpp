#pragma once

#include <wfc/io/ip/tcp/rn/client_options.hpp>
#include <wfc/io/ip/tcp/rn/connection.hpp>
#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>


namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

struct _connection_type_;
  
struct client_aspect: 
  fas::aspect<
    fas::type<_connection_type_, connection>,
    fas::advice< wfc::io::_options_type_, client_options>,
    fas::stub< wfc::io::_stop_ >
  >
{
};
  
}}}}}