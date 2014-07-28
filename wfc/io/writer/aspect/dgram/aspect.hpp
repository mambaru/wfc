#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <wfc/io/writer/aspect/dgram/ad_async_send_to.hpp>
#include <wfc/io/writer/aspect/dgram/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace dgram{
  
struct aspect: fas::aspect<
  fas::alias<  _remote_endpoint_, ::wfc::io::reader::_remote_endpoint_ >, 
  fas::advice< _write_some_,    ad_async_send_to >
>{};
  

}}}}
