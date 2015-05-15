#pragma once

#include <wfc/io/writer/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ 
  
struct ad_log_abort
{
  template<typename T>
  void operator()(T& , boost::system::error_code ec)
  {
    ::wfc::only_for_log(ec);
    TRACE_LOG_WARNING( "WRITER aborted: "  << ec.value() << " " << ec.message() )
  }
};

}}}