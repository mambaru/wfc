#pragma once

#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_read_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, boost::system::error_code ec , std::size_t bytes_transferred)
  {
    if ( !t.status() )
      return;
            
    if ( !ec )
    {
      d->resize(bytes_transferred);
      t.get_aspect().template get<_read_ready_>()(t, std::move(d));
    }
    else
    {
      t.get_aspect().template get<_read_error_>()(t, std::move(d), ec);
    }
  }
};

}}}