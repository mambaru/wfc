#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    t.dispatch( [&t]() {
      //t.get_aspect().template get<_is_started_>() = true; // TODO: нахуй
      t.get_aspect().template gete<_read_more_>()(t);
    });
  }
};

}}}