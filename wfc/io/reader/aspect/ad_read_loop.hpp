#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_read_loop
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    // user handler
    t.get_aspect().template get<_incoming_>()(t, std::move(d) );
    // loop (_read_more_)
    t.get_aspect().template get<_read_more_>()(t);
  }
};

}}}
