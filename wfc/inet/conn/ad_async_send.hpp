#pragma once

#include <wfc/inet/types.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{
  
struct ad_async_send
{
  template<typename T, typename F>
  void operator()(T& t, const data_type& d, F callback)
  {
    t.socket().async_send( 
      ::boost::asio::buffer( d.data(),  d.size() ), 
      t.strand().wrap(callback)
    );
  }
};


}}
