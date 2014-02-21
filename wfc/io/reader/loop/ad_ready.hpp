#pragma once

#include <wfc/io/reader/loop/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace loop{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "loop ready" << std::endl;
    t.get_aspect().template get< _outgoing_ >()(t, std::move(d));
    t.get_aspect().template get< _more_ >()(t);
  }
};

}}}}
