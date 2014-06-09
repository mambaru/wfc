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
  
  connection_base(descriptor_type&& desc, const options_type& conf/*, wfc::io::incoming_handler handler = nullptr*/)
    : super( std::move(desc), conf/*, handler*/)
  {
  }
};
  
}}}}}
