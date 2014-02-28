#pragma once

#include <wfc/logger.hpp>
#include <string>

namespace wfc{ namespace io{ namespace rn{ namespace reader{ namespace trace{

struct ad_trace
{
#ifndef NDEBUG
  template<typename T,  typename Itr>
  void operator()(T& , Itr beg, Itr end)
  {
    TRACE_LOG_MESSAGE( "READER RN TRACE [[" << std::string(beg, end) << "]]" )
  }
#else 
  template<typename T,  typename Itr>
  void operator()(T& , Itr , Itr ) {}
#endif
};

}}}}}
