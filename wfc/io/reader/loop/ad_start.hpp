#pragma once

#include <wfc/io/reader/loop/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace loop{

struct ad_start
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "loop start" << std::endl;
    t.dispatch( [&t]() {
      std::cout << "loop started" << std::endl;
      t.get_aspect().template gete<_on_start_>()(t);
      t.get_aspect().template get<_more_>()(t);
    });
  }
};

}}}}
