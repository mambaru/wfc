#pragma once
#include <boost/asio.hpp>


namespace wfc{ namespace inet{ namespace conn{

struct ad_async_receive
{
  template<typename T, typename DataType, typename F>
  void operator()(T& t, DataType& d, F callback)
  {
    // TODO: нужно ди owner
    
    t.socket().async_receive( 
      ::boost::asio::buffer( d ), 
      t.strand().wrap( callback )
    );
  }
};

}}}
