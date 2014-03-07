#pragma once

#include <wfc/io/writer/basic/tags.hpp>
#include <wfc/io/writer/errors/tags.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/error.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{

struct ad_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, boost::system::error_code ec, std::size_t bytes_transferred)
  {
    if (!ec)
    {
      t.get_aspect().template get<_ready_>()( t, std::move(d), bytes_transferred);
    }
    else if ( ec == boost::asio::error::operation_aborted)
    {
      std::cout << "ABORT" << std::endl;
      t.get_aspect().template get< errors::_aborted_ >()(t, std::move(d) );
    }
    else
    {
      std::cout << "ERROR" << std::endl;
      t.get_aspect().template get< errors::_error_ >()(t, ec, std::move(d) );
    }
  }
};

}}}}
