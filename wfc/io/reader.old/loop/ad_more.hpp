#pragma once

#include <wfc/io/reader/loop/tags.hpp>
#include <wfc/io/reader/common/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace loop{

struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get< common::_make_buffer_ >()(t);
    t.get_aspect().template get< _read_some_ >()(t, std::move(d) );
  }
};

}}}}
