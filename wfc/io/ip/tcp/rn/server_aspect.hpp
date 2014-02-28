#pragma once


#include <wfc/io/ip/tcp/rn/acceptor.hpp>
#include <wfc/io/ip/tcp/rn/server_options.hpp>

#include <wfc/io/server/tags.hpp>
#include <wfc/io/server/server.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{
  
struct server_aspect: fas::aspect
<
  ::wfc::io::server::aspect<acceptor>,
  fas::type<::wfc::io::_options_type_, server_options>,
  fas::stub< wfc::io::server::_create_ >,
  fas::stub< wfc::io::server::_configure_ >,
  fas::stub< wfc::io::server::_reconfigure_ >,
  fas::stub< wfc::io::server::_start_ >,
  fas::stub< wfc::io::server::_stop_ >
  
>{};
  
}}}}}