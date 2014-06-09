#pragma once

#include <wfc/logger.hpp>

#include <wfc/io/acceptor/basic/tags.hpp>
//#include <wfc/io/acceptor/errors/tags.hpp>

#include <boost/system/error_code.hpp>
#include <boost/asio/error.hpp>


namespace wfc{ namespace io{ namespace acceptor{ namespace basic{

struct ad_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, boost::system::error_code ec)
  {
    if (!ec)
    {
      TRACE_LOG_MESSAGE("acceptor::handler READY:" << ec.message())
      t.get_aspect().template get<_ready_>()( t, std::move(d) );
    }
    else if ( ec == boost::asio::error::operation_aborted)
    {
      TRACE_LOG_MESSAGE("acceptor::handler operation_aborted")
      ///!!!!!!!!! t.get_aspect().template get< errors::_aborted_ >()(t, std::move(d) );
    }
    else
    {
      TRACE_LOG_MESSAGE("acceptor::handler error:" << ec.message())
      ///!!!!!!!!!  t.get_aspect().template get< errors::_error_ >()(t, ec, std::move(d) );
    }
  }
};

}}}}
