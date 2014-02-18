#pragma once
#include <boost/asio.hpp>
#include <wfc/io/reader/tags.hpp>
namespace wfc{ namespace io{ namespace reader{

struct ad_async_read_some
{
  template<typename T>
  void operator()(T& t)
  {
    typedef typename T::data_type data_type;
    data_type& d = t.get_aspect().template get<_buffer_manager_>().create(t);
    
    t.get_aspect().template get<_descriptor_ptr_>()->async_read_some
    (
      ::boost::asio::buffer( d ), 
      /*t.get_aspect().template get<_wrap_>()
      (
        t,
     */
        [&t, &d]( boost::system::error_code ec, std::size_t bytes_transferred )
        {
          
          if (!ec)
          {
            auto pd = t.get_aspect().template get<_buffer_manager_>().pop(t);
            pd->resize( bytes_transferred);
            t.get_aspect().template get<_async_read_>()( t, std::move(pd));
          }
          else if ( ec == boost::asio::error::operation_aborted)
          {
            // TODO: async aborted
            t.get_aspect().template get<_aborted_>()(t, d);
          }
          else
          {
            t.get_aspect().template get<_error_>()(t, ec, d);
          }
        } // lambda
      //) // _wrap_
    );// _read_descriptor_
  }
};

}}}
