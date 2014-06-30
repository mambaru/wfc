#pragma once

#include <boost/asio.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ 
  
struct ad_read_error
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, boost::system::error_code ec)
  {
    DEBUG_LOG_BEGIN("ad_read_error")
    t.get_aspect().template get<_status_ >()= false;
    
    if ( ec == boost::asio::error::operation_aborted)
    {
      t.get_aspect().template get< _on_abort_ >()(t, ec );
    }
    else
    {
      DEBUG_LOG_BEGIN("ad_read_error _on_error_")
      t.get_aspect().template get< _on_error_ >()(t, ec );
      DEBUG_LOG_END("ad_read_error _on_error_")
    }
    
    t.get_aspect().template get< _free_buffer_ >()(t, std::move(d));
    
    // Ахтунг!!! вызываеться в connection по своим правилам
    // t.self_stop(t, nullptr);
    DEBUG_LOG_END("ad_read_error")
  }
};

}}}