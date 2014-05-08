#pragma once

#include <wfc/io/rn/reader/tags.hpp>
#include <utility>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template gete<_on_read_>()(t, d->begin(), d->end());
    t.get_aspect().template get<_outgoing_>()(t, std::move(d));
  }
};

}}}}
