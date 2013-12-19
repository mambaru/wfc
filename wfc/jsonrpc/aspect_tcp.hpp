#pragma once 

#include <wfc/jsonrpc/aspect.hpp>

#include <wfc/inet/stream/aspect.hpp>
#include <wfc/inet/rn/aspect.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/inet/conn/connection_context.hpp>

namespace wfc{ namespace jsonrpc{

struct aspect_tcp: fas::aspect<
  inet::context< inet::connection_context>, 
  inet::stream::aspect,
  inet::rn::aspect,
  aspect,
  fas::alias< inet::stream::_incoming_, inet::rn::_input_        >,
  fas::alias< inet::rn::_output_,       inet::stream::_outgoing_ >,
  fas::alias< inet::rn::_incoming_,     jsonrpc::_input_   >,
  fas::alias< jsonrpc::_output_,        inet::rn::_outgoing_     >
>{};

}}
