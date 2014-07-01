#pragma once

#include <boost/asio.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
 
struct ad_read_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template gete<_on_read_>()(t, d->begin(), d->end() );
    t.get_aspect().template get<_read_loop_>()(t, std::move(d) );
  }
};

}}}