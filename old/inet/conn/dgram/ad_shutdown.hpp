#pragma once

/*
#include <wfc/inet/conn/ad_connection_start.hpp>
#include <wfc/inet/conn/ad_stream_reader.hpp>
#include <wfc/inet/conn/ad_stream_writer.hpp>
#include <wfc/inet/conn/connection_context.hpp>
#include <wfc/inet/stream/tags.hpp>
#include <wfc/inet/stream/context.hpp>
#include <wfc/inet/stream/ad_outgoing.hpp>
#include <wfc/inet/stream/ad_shutdown.hpp>
#include <wfc/inet/stream/ad_on_write.hpp>
#include <wfc/inet/conn/tags.hpp>
#include <wfc/inet/types.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/logger.hpp>
#include <fas/aop.hpp>
*/

namespace wfc{ namespace inet{ namespace dgram{


// ex: ad_fake_shutdown
struct ad_shutdown
{
  template<typename T>
  void operator()(T& t)
  {
    // std::cout << "ad_shutdown" << std::endl;
    t.get_io_service().dispatch([&t]()
    {
      if ( t.context().shutdown )
        return;
      //t.socket().shutdown( boost::asio::socket_base::shutdown_receive);
      // std::cout << "ad_shutdown dispatch" << std::endl;
      size_t size  = t.get_aspect().template get<_writer_>().outgoing_size();
      if ( size == 0)
      {
        t.close();
      }
      else
      {
        // std::cout << "t.context().shutdown == true" << std::endl;
        t.context().shutdown = true;
      }
    });
  }
};


}}}
