#pragma once

#include <wfc/io/reader/common/tags.hpp>
#include <wfc/io/reader/errors/tags.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/error.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace common{

struct ad_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, boost::system::error_code ec/*, std::size_t bytes_transferred*/)
  {
    if (!ec)
    {
      // d->resize( bytes_transferred);
      t.get_aspect().template get<_ready_>()( t, std::move(d) );
    }
    else if ( ec == boost::asio::error::operation_aborted)
    {
      t.get_aspect().template get< errors::_aborted_ >()(t, std::move(d) );
    }
    else
    {
      t.get_aspect().template get< errors::_error_ >()(t, ec, std::move(d) );
    }
  }
};

}}}}
