#pragma once


#include <wfc/io/writer/basic/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{

struct ad_write
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    d = t.get_aspect().template get<_make_buffer_>()(t, std::move(d) );
    
    if ( d != nullptr )
      t.get_aspect().template get< _write_some_ >()(t, std::move(d) );

  }
};

}}}}
