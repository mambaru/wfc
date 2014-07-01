#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace writer{

struct ad_write_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get< _make_buffer_ >()(t, nullptr);
    if ( d != nullptr )
    {
      t.get_aspect().template get< _write_some_ >()(t, std::move(d) );
    }
  }
};

}}}
