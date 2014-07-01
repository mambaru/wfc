#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <boost/system/error_code.hpp>

namespace wfc{ namespace io{ namespace writer{
  
struct ad_write_error
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr, boost::system::error_code ec, std::size_t /*bytes_transferred*/)
  {
    t.get_aspect().template get<_status_ >() = false;
    
    if ( ec == boost::asio::error::operation_aborted)
    {
      t.get_aspect().template gete< _on_abort_ >()(t, ec );
    }
    else
    {
      t.get_aspect().template gete< _on_error_ >()(t, ec );
    }
    
    // t.get_aspect().template get< _free_buffer_ >()(t, std::move(d));
  }
};

}}}
