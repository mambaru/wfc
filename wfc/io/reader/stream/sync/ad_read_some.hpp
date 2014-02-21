#pragma once

#include <wfc/io/reader/errors/tags.hpp>
#include <wfc/io/reader/stream/sync/tags.hpp>
#include <wfc/io/tags.hpp>

#include <boost/asio.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace stream{ namespace sync{

struct ad_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    boost::system::error_code ec;
    
    size_t bytes_transferred = 
      t.get_aspect().template get<_descriptor_ptr_>()->read_some( ::boost::asio::buffer( *d ), ec);
      
    if (!ec)
    {
      d->resize(bytes_transferred);
      t.get_aspect().template get<_ready_>()( t, std::move(d) );
    }
    else if ( ec == boost::asio::error::operation_aborted)
    {
      t.get_aspect().template get<errors::_aborted_>()(t, std::move(d));
    }
    else
    {
      t.get_aspect().template get<errors::_error_>()(t, ec, std::move(d));
    }
  }
};

}}}}}
