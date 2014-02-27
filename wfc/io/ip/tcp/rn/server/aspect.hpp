#pragma once


#include "acceptor.hpp"
#include <wfc/io/server/tags.hpp>
#include <wfc/io/server/server.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace impl{ 
  
struct aspect1: fas::aspect
<
  ::wfc::io::server::aspect<acceptor>,
  fas::stub< wfc::io::server::_create_ >,
  fas::stub< wfc::io::server::_configure_ >,
  fas::stub< wfc::io::server::_reconfigure_ >,
  fas::stub< wfc::io::server::_start_ >,
  fas::stub< wfc::io::server::_stop_ >
  
>{};
  
}}}}}}