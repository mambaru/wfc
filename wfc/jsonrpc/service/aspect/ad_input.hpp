#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/jsonrpc/service/aspect/tags.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/io/types.hpp>
#include <wfc/logger.hpp>
#include <memory>

namespace wfc{ namespace jsonrpc{
  
struct ad_input
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d, io::io_id_t io_id, io::outgoing_handler_t outgoing_handler)
  {
    typedef std::chrono::high_resolution_clock clock_t;
    clock_t::time_point now = clock_t::now();
    
    ::wfc::io::outgoing_handler_t tp_callback = [now, outgoing_handler]( ::wfc::io::data_ptr d)
    {
      outgoing_handler( std::move(d) );
    };

    auto dd = std::make_shared< typename T::data_ptr >( std::move(d) );

    t.post([&t, dd, io_id, tp_callback]()
    {
      t.get_aspect().template get<_incoming_parser_>()( t, std::move(*dd), io_id, std::move(tp_callback) );
    });

  }
};

  
}} // wfc


