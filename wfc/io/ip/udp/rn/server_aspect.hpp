#pragma once


#include <wfc/io/ip/udp/rn/acceptor.hpp>
#include <wfc/io/ip/udp/rn/server_options.hpp>

#include <wfc/io/server/tags.hpp>
#include <wfc/io/server/server.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{
  
struct server_aspect: fas::aspect
<
  ::wfc::io::server::aspect<acceptor>,
  fas::type< ::wfc::io::_options_type_, server_options>,
  fas::stub< ::wfc::io::server::_create_ >,
  fas::stub< ::wfc::io::server::_configure_ >,
  fas::stub< ::wfc::io::server::_reconfigure_ >,
  fas::stub< ::wfc::io::server::_start_ >,
  fas::stub< ::wfc::io::server::_stop_ >
>{};
  
}}}}}
