#pragma once

#include <boost/asio.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_read_error
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, boost::system::error_code ec)
  {
    t.get_aspect().template get<_status_ >()= false;
    
    if ( ec == boost::asio::error::operation_aborted)
    {
      t.get_aspect().template gete< _on_abort_ >()(t, ec );
    }
    else
    {
      t.get_aspect().template gete< _on_error_ >()(t, ec );
    }
    
    t.get_aspect().template get< _free_buffer_ >()(t, std::move(d));
  }
};

}}}
