#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>

#include <wfc/io/reader/errors/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace errors{ 

struct ad_error
{
  template<typename T>
  void operator()(T& t, boost::system::error_code ec, typename T::data_ptr /*d*/)
  {
    t.get_aspect().template gete<_on_error_>()(t, ec);
  }
};

}}}}
