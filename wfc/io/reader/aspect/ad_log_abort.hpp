#pragma once

#include <wfc/io/reader/aspect/tags.hpp>
#include <wfc/logger.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_log_abort
{
  template<typename T>
  void operator()(T& , boost::system::error_code ec)
  {
    ::wfc::only_for_log(ec);
    TRACE_LOG_WARNING( "READER aborted: "  << ec.value() << " " << ec.message() )
  }
};

}}}