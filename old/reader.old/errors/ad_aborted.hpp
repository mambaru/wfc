#pragma once

#include <wfc/io/reader/errors/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace errors{ 

struct ad_aborted
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr /*d*/)
  {
    t.get_aspect().template gete<_on_aborted_>()(t);
  }
};

}}}}
