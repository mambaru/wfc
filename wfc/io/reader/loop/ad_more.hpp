#pragma once

#include <wfc/io/reader/async/loop/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace loop{

struct ad_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get<common::_make_buffer_>()(t);
    t.get_aspect().template get< wfc::io::reader::async::_async_read_some_ >()(t, std::move(d) );
  }
}

}}}}
