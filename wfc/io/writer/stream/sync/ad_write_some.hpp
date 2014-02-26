#pragma once

#include <wfc/io/writer/stream/sync/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace stream{ namespace sync{

struct ad_write_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    boost::system::error_code ec;
    
    size_t bytes_transferred = 
      t.descriptor().write_some( ::boost::asio::buffer( *d ), ec);
    t.get_aspect().template get< _outgoing_ >()(t, std::move(d), ec, bytes_transferred);
  }
};

}}}}}
