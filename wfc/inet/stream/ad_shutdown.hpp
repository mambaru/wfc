#pragma once

#include <wfc/inet/stream/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace stream{

struct ad_shutdown
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_io_service().dispatch([&t]()
    {
      if ( t.context().shutdown )
        return;
      t.socket().shutdown( boost::asio::socket_base::shutdown_receive);
      size_t size  = t.get_aspect().template get<_writer_>().outgoing_size();
      if ( size == 0)
      {
        t.close();
      }
      else
      {
        t.context().shutdown = true;
      }
    });
  }
};

}}}
