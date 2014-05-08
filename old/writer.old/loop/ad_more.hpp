#pragma once

#include <wfc/io/writer/loop/tags.hpp>
#include <wfc/io/writer/basic/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace loop{

struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get< basic::_make_buffer_ >()(t, nullptr);
    if ( d != nullptr )
    {
      t.get_aspect().template get< _write_some_ >()(t, std::move(d) );
    }
  }
};

}}}}
