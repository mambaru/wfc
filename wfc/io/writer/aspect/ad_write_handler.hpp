#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <boost/system/error_code.hpp>

namespace wfc{ namespace io{ namespace writer{
  

struct ad_write_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, boost::system::error_code ec, std::size_t bytes_transferred)
  {
    if ( !t.status() )
    {
      DAEMON_LOG_FATAL("wfc::io::writer::ad_write_handler !t.status()")
      return;
    }
    
    if ( !ec )
    {
      t.get_aspect().template get<_write_ready_>()(t, std::move(d), bytes_transferred);
    }
    else
    {
      t.get_aspect().template get<_write_error_>()(t, std::move(d), ec, bytes_transferred);
    }
  }
};


}}}
