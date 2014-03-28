#pragma once

#include <wfc/inet/types.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace conn{
  
struct ad_send_to
{
  template<typename T>
  size_t operator()(T& t, const data_type& d)
  {
    //return 0;
    boost::system::error_code ec;
    size_t bytes_transferred = t.socket().send_to( 
      ::boost::asio::buffer(d.data(), d.size()), 
      t.remote_endpoint(), 
      0, ec
    );
    
    // TODO: проверка на cancel
    if (ec)
    {
      bytes_transferred = 0;
      t.get_aspect().template get<_write_error_>()(t,  ec);
      t.close();
    }
    else
    {
    }
    
    return bytes_transferred;
  }
};

}}}
