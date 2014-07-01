#pragma once

#include <wfc/logger.hpp>
#include <string>

namespace wfc{ namespace io{ namespace writer{
  
struct ad_trace
{
  template<typename T, typename Itr>
  void operator()(T& , Itr beg, Itr end)
  {
    only_for_log(beg, end);
    TRACE_LOG_MESSAGE( "WRITE [[" << std::string(beg, end ) << "]] " )
  }
};

}}}
