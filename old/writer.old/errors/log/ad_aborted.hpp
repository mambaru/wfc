#pragma once

#include <wfc/logger.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace errors{ namespace log{

struct ad_aborted
{
  template<typename T>
  void operator()(T&)
  {
    COMMON_LOG_WARNING( "WRITER aborted" )
  }
};

}}}}}
