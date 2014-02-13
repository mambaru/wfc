#pragma once 

#include <fas/aop.hpp>
#include <efc/inet/srv/ip/tcp/types.hpp>
#include <efc/inet/srv/tags.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
  
struct aspect: fas::aspect
<
  fas::type < _acceptor_type_, acceptor_type>,
  fas::value< _acceptor_, acceptor_ptr >
>
{};
  
}}}}}
