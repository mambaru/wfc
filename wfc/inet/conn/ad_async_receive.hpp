#pragma once
#include <boost/asio.hpp>


namespace wfc{ namespace inet{

struct ad_async_receive
{
  template<typename T, typename DataType, typename F>
  void operator()(T& t, DataType& d, F callback)
  {
    t.socket().async_receive( 
      ::boost::asio::buffer( d ), 
      t.strand().wrap( callback )
    );
  }
};

}}
