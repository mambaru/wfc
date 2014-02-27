#pragma once
#include <wfc/io/connection/connection.hpp>
#include <wfc/io/strategy/posix/writer/config.hpp>
#include <wfc/io/strategy/posix/connection/rn/stream.hpp>

#include <fas/aop.hpp>

#include <wfc/io/ip/tcp/rn/server/config.hpp>
#include <wfc/io/tags.hpp>
namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace impl{ 

struct ad_reverse
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    std::reverse( d->begin(), d->end() );
    t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
    t.descriptor().close();
  }
};

  
struct connection_aspect: 
  fas::aspect<
    fas::advice< wfc::io::_options_type_, config>,
    fas::advice< wfc::io::_incoming_, ad_reverse>,
    fas::type< wfc::io::_descriptor_type_, boost::asio::ip::tcp::socket>,
    wfc::io::strategy::posix::connection::rn::stream
  >
{
};

class connection:
  public wfc::io::connection::connection<connection_aspect>
{
  typedef wfc::io::connection::connection<connection_aspect> super;
public:
  template<typename Conf>
  connection(descriptor_type&& desc, const Conf& conf)
    : super( std::move(desc), conf )
  {
  }
};
  
}}}}}}