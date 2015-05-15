#pragma once

#include <wfc/jsonrpc/service/aspect/tags.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_add_shutdown
{
  template<typename T, typename AddShutdown>
  void operator()( T& t, AddShutdown add_shutdown)
  {
    typedef typename T::io_id_t io_id_t;
    add_shutdown([&t]( io_id_t io_id )
    {
      if ( auto handler = t.registry().erase_io(io_id) )
      {
        handler->stop(io_id);
      }
    });
  }
};
  
}} // wfc
