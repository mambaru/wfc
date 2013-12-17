#pragma once
#include <wfc/inet/stream/aspect.hpp>
#include <wfc/inet/rn/aspect.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/inet/connection_context.hpp>

namespace wfc{ namespace inet{ namespace echo{

struct aspect_stream_rn: fas::aspect<
  stream::aspect,
  rn::aspect,
  context<connection_context>, // ?? в stream
  fas::alias<stream::_incoming_, rn::_input_>,
  fas::alias<rn::_output_, stream::_outgoing_>,
  fas::alias<rn::_incoming_, rn::_outgoing_>
>{};
  
}}}
