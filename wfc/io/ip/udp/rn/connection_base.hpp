#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/udp/rn/connection_aspect.hpp>
#include <wfc/io/connection/connection.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{ 

template<typename A = fas::aspect<> >
class connection_base:
  public wfc::io::connection::connection< typename fas::merge_aspect<A, connection_aspect>::type >
{
  typedef wfc::io::connection::connection< typename fas::merge_aspect<A, connection_aspect>::type > super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::mutex_type mutex_type;
  typedef typename super::lock_guard lock_guard;
  
  connection_base(descriptor_type&& desc, const options_type& opt)
    : super( std::move(desc), opt)
  {
  }
  
  void listen()
  {
    COMMON_LOG_MESSAGE("udp \"listen\" " << super::options().host << ":" << super::options().port)
    
    boost::asio::ip::udp::resolver resolver( super::get_io_service() );
    boost::asio::ip::udp::endpoint endpoint = *resolver.resolve({
      super::options().host, 
      super::options().port
    });

    super::descriptor().open( endpoint.protocol() );
    
    boost::asio::socket_base::receive_buffer_size option(16777216 * 4);
    super::descriptor().set_option(option);
    
    //super::descriptor().set_option( boost::asio::ip::udp::acceptor::reuse_address(true) );
    super::descriptor().bind( endpoint );

  }
  
};
  
}}}}}
