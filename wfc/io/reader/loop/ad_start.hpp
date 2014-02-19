#pragma once

#include <wfc/io/reader/async/loop/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace loop{

struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template gete<_on_start_>()(t);
    t.get_aspect().template get<_more_>()(t);
  }
}

}}}}
