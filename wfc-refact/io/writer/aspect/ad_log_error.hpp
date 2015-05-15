#pragma once

#include <wfc/io/writer/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ 
  
struct ad_log_error
{
  template<typename T>
  void operator()(T& , boost::system::error_code ec)
  {
    if( ec.value() != boost::system::errc::no_such_file_or_directory )
    {
      COMMON_LOG_ERROR( "WRITER error: " << ec.value() << " " << ec.message() )
    }
    else
    {
      TRACE_LOG_MESSAGE( "WRITER error: " << ec.value() << " " << ec.message() )
    }
  }
};

}}}