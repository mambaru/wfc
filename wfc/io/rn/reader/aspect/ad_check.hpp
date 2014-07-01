#pragma once

#include <wfc/io/rn/reader/aspect/tags.hpp>
//#include <wfc/io/rn/reader/check/tags.hpp>
#include <utility>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

struct ad_check
{
  template<typename T>
  void operator()(T& t)
  {
    auto& buffer = t.get_aspect().template get<_buffer_>();
    if ( buffer != nullptr )
    {
      if ( buffer->size() > t.options().rn_limit_error )
      {
        t.get_aspect().template gete<_on_limit_error_>()(t, buffer->size() );
      }
      else if ( buffer->size() > t.options().rn_limit_warning )
      {
        t.get_aspect().template gete<_on_limit_warning_>()(t, buffer->size());
      }
    }
  }
};

}}}}
