#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_read_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get< _make_buffer_ >()(t);
    t.get_aspect().template get< _read_some_ >()(t, std::move(d) );
  }
};

}}}