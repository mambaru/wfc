#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace writer{
  
 
struct ad_write
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template get<_outgoing_buffer_size_>()+=d->size();
    d = t.get_aspect().template get<_make_buffer_>()(t, std::move(d) );
    
    if ( d != nullptr )
      t.get_aspect().template get< _write_some_ >()(t, std::move(d) );
  }
};


}}}
