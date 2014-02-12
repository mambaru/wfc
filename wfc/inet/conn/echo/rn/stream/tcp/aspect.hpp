#pragma once
#include <wfc/inet/conn/rn/stream/tcp/aspect.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace echo{ namespace rn{ namespace stream{ namespace tcp{

struct aspect: fas::aspect<
  fas::alias<wfc::inet::conn::rn::_incoming_, wfc::inet::conn::rn::_outgoing_>,
  wfc::inet::conn::rn::stream::tcp::aspect
>{};

}}}}}}}
