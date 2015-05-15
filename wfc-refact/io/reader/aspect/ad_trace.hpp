#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_trace
{
  template<typename T, typename Itr>
  void operator()(T& , Itr beg, Itr end)
  {
    only_for_log(beg, end);
    TRACE_LOG_MESSAGE( "READ [[" << std::string(beg, end ) << "]] " )
  }
};

}}}