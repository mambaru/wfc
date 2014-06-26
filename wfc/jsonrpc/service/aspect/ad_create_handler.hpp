#pragma once

#include <wfc/jsonrpc/service/aspect/tags.hpp>


namespace wfc{ namespace jsonrpc{
  
struct ad_create_handler
{
  template<typename T>
  void operator()( T& t, typename T::io_id_t io_id, typename T::outgoing_handler_t outgoing_handler)
  {
    /*t.post([&t, io_id, outgoing_handler]()*/
    {
      if ( auto handler = t.clone_prototype() )
      {
        t.get_aspect().template get<_handler_start_>()(t, io_id, *handler);
        t.registry().set_io( io_id, handler, outgoing_handler );
      }
    }/*)*/;
  }
};

  
}} // wfc


