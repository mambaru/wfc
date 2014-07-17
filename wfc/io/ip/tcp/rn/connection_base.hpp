#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/tcp/rn/connection_aspect.hpp>
#include <wfc/io/connection/connection.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 

template<typename A = fas::aspect<> >
class connection_base:
  public wfc::io::connection::connection< typename fas::merge_aspect<A, connection_aspect>::type >
{
  typedef wfc::io::connection::connection< typename fas::merge_aspect<A, connection_aspect>::type > super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;
  
  connection_base(descriptor_type&& desc, const options_type& opt)
    : super( std::move(desc), opt)
  {
    boost::asio::ip::tcp::no_delay no_delay(true);
    super::descriptor().set_option(no_delay);
    
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    super::descriptor().io_control(non_blocking_io);

    boost::asio::socket_base::keep_alive keep_alive(opt.keep_alive);
    super::descriptor().set_option(keep_alive);

  }
};
  
}}}}}
