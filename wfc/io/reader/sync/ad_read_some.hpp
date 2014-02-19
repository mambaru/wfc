#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/io/reader/sync/tags.hpp>
#include <wfc/io/reader/errors/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace sync{

struct ad_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "ad_read_some [" << std::endl;
    boost::system::error_code ec;
    
    size_t bytes_transferred = 
      t.get_aspect().template get<_descriptor_ptr_>()->read_some( ::boost::asio::buffer( *d ), ec);
      
    std::cout << "]ad_read_some " << std::endl;
    if (!ec)
    {
      std::cout << "data size " << d->size() << std::endl;
      std::cout << "bytes_transferred " << bytes_transferred << std::endl;
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

}}}}
