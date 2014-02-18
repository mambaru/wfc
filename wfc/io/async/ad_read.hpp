#pragma once
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace async{

struct ad_read
{
  template<typename T, typename DataType, typename F>
  void operator()(T& t, DataType& d, F callback)
  {
    t.get_aspect().template get<_read_descriptor_>()->async_read_some
    (
      ::boost::asio::buffer( d ), 
      t.get_aspect().template get<_wrap_>()
      (
        t,
        [&t, &d]( boost::system::error_code ec, std::size_t bytes_transferred )
        {
          
          if (!ec)
          {
            callback(bytes_transferred);
          }
          else if ( ec == boost::asio::error::operation_aborted)
          {
            t.get_aspect().template get<_read_aborted_>()(t, d);
          }
          else
          {
            t.get_aspect().template get<_read_error_>()(t, ec, d);
          }
        } // lambda
      ) // _wrap_
    );// _read_descriptor_
  }
};

}}}
