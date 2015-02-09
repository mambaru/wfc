#pragma once

#include <wfc/io/reader/aspect/tags.hpp>
#include <boost/system/error_code.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_log_error
{
  template<typename T>
  void operator()(T& , boost::system::error_code ec)
  {
    if( ec.value() != boost::system::errc::no_such_file_or_directory )
    {
      COMMON_LOG_ERROR( "READER error: " << ec.value() << " " << ec.message() )
    }
    else
    {
      TRACE_LOG_MESSAGE( "READER error: " << ec.value() << " " << ec.message() )
    }
  }
};

}}}