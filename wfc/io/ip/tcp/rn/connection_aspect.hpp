#pragma once

#include <wfc/io/ip/tcp/rn/connection_options.hpp>
#include <wfc/io/strategy/posix/connection/rn/stream.hpp>
#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

// Убрать нафик (поставить handler)
struct ad_handler
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    auto handler = t.options().handler;
    if ( handler == nullptr )
    {
      t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
    }
    else
    {
      handler( std::move(d), [&t](typename T::data_ptr d)
      {
        t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
        t.descriptor().close();
      });
    }
    /*
    std::reverse( d->begin(), d->end() );
    t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
    t.descriptor().close();
    */
  }
};

  
struct connection_aspect: 
  fas::aspect<
    fas::stub< wfc::io::_stop_>,
    fas::advice< wfc::io::_options_type_, connection_options>,
    fas::advice< wfc::io::_incoming_, ad_handler>,
    fas::type< wfc::io::_descriptor_type_, boost::asio::ip::tcp::socket>,
    wfc::io::strategy::posix::connection::rn::stream
  >
{
};
  
}}}}}