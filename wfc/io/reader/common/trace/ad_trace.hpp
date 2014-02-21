#pragma once

#include <wfc/logger.hpp>
#include <string>

namespace wfc{ namespace io{ namespace reader{ namespace common{ namespace trace{

struct ad_trace
{
  template<typename T,  typename Itr>
  void operator()(T& , Itr beg, Itr end)
  {
    TRACE_LOG_MESSAGE( "READER TRACE [[" << std::string(beg, end) << "]]" )
  }
};

}}}}}
