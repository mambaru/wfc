#pragma once
#include <wfc/inet/stream/aspect.hpp>
#include <wfc/inet/rn/aspect.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/inet/conn/connection_context.hpp>

namespace wfc{ namespace inet{ namespace echo{

struct aspect_stream_rn: fas::aspect<
  stream::aspect,
  rn::aspect,
  fas::alias<stream::_incoming_, rn::_input_>,
  fas::alias<rn::_output_, stream::_outgoing_>,
  fas::alias<rn::_incoming_, rn::_outgoing_>
>{};

struct aspect_dgram_rn: fas::aspect<
  stream::aspect_dgram,
  rn::aspect,
  fas::alias<stream::_incoming_, rn::_input_>,
  fas::alias<rn::_output_, stream::_outgoing_>,
  fas::alias<rn::_incoming_, rn::_outgoing_>
>{};

}}}
