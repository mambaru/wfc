#pragma once

#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace jsonrpc{
  
struct ad_handler_start
{
  template<typename T>
  void operator()( T& t, typename T::io_id_t io_id, typename T::handler_interface& handler)
  {
    using namespace std::placeholders; 

    handler.send_request = std::bind(&T::send_request, &t, io_id, _1, _2, _3 );
      
    handler.send_notify = std::bind(&T::send_notify, &t, io_id, _1, _2);
   
    handler.start(io_id);
  }
};

  
}} // wfc


