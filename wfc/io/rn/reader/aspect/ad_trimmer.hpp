#pragma once

#include <wfc/io/rn/reader/aspect/tags.hpp>
#include <cstring>
#include <utility>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

struct ad_trimmer
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    static size_t size = std::strlen( t.get_aspect().template get<_separator_>()() );
    
    if ( d->size() >= size )
      d->resize( d->size() - size );
    else
      d->clear();
    
    t.get_aspect().template get<_ready_>()(t, std::move(d));
  }
};

}}}}
