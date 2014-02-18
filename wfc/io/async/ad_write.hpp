#pragma once

#include <wfc/inet/types.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace async{
  
struct ad_write
{
  template<typename T, typename DataType, typename F>
  void operator()(T& t, const DataType& d, F callback)
  {
    t.get_aspect().template get<_write_descriptor_>()->async_write_some( 
      ::boost::asio::buffer( d), 
      t.get_aspect().template get<_wrap_>()(t, callback )
    );
  }
};

}}}
