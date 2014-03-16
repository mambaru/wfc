#pragma once

#include <wfc/logger.hpp>
#include <boost/system/error_code.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace errors{ namespace log{

struct ad_error
{
  template<typename T>
  void operator()(T& , boost::system::error_code ec)
  {
    COMMON_LOG_ERROR( "READER error: " << ec.message() )
  }
};

}}}}}
