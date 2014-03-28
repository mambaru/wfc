#pragma once

#include <wfc/logger.hpp>

namespace wfc{ namespace io{ namespace rn{ namespace reader{ namespace log{

struct ad_limit_error
{
  template<typename T>
  void operator()(T& , size_t s)
  {
    COMMON_LOG_ERROR( "READER RN limit: " << s << " bytes in incoming buffer" )
  }
};

}}}}}
