#pragma once
#include <wfc/inet/conn/stream/udp/aspect.hpp>
#include <wfc/inet/conn/rn/aspect.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace rn{ namespace stream{ namespace udp{

struct aspect: fas::aspect<
  fas::alias< wfc::inet::conn::_incoming_, wfc::inet::conn::rn::_input_>,
  fas::alias< wfc::inet::conn::rn::_output_, wfc::inet::conn::stream::_outgoing_>,
  wfc::inet::conn::rn::aspect,
  wfc::inet::conn::stream::udp::aspect
>
{};

}}}}}}
