#pragma once

#include <wfc/io/rn/reader/tags.hpp>
#include <wfc/io/rn/reader/check/tags.hpp>
#include <utility>


namespace wfc{ namespace io{ namespace rn{ namespace reader{ namespace check{

struct ad_check
{
  template<typename T>
  void operator()(T& t)
  {
    auto& buffer = t.get_aspect().template get<_buffer_>();
    if ( buffer != nullptr )
    {
      if ( buffer->size() > t.get_aspect().template get<_limit_error_>() )
      {
        t.get_aspect().template gete<_on_limit_error_>()(t, buffer->size() );
      }
      else if ( buffer->size() > t.get_aspect().template get<_limit_warning_>() )
      {
        t.get_aspect().template gete<_on_limit_warning_>()(t, buffer->size());
      }
    }
  }
};

}}}}}
