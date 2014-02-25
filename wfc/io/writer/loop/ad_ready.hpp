#pragma once

#include <wfc/io/writer/loop/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace loop{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, std::size_t bytes_transferred)
  {
    // todo: stub
    t.get_aspect().template get< _outgoing_ >()(t, bytes_transferred);
    t.get_aspect().template get< _more_ >()(t);
  }
};

}}}}
