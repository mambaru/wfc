#pragma once

#include <wfc/io/ip/udp/rn/connection.hpp>
#include <wfc/io/ip/udp/rn/acceptor_options.hpp>
#include <wfc/io/acceptor/acceptor.hpp>
#include <wfc/io/server/tags.hpp>
#include <fas/aop.hpp>


#include <set>



namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{
  
struct acceptor_aspect: 
  fas::aspect<
    fas::type< _descriptor_type_, boost::asio::ip::udp::socket>
  >
{};

}}}}}
