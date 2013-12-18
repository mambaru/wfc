#pragma once 

#include <wfc/inet/stream/aspect.hpp>
#include <wfc/inet/rn/aspect.hpp>
#include <wfc/inet/jsonrpc/aspect.hpp>
#include <wfc/inet/context.hpp>
#include <wfc/inet/conn/connection_context.hpp>

namespace wfc{ namespace jsonrpc{

struct aspect: fas::aspect<
  inet::context< inet::connection_context>, 
  inet::stream::aspect,
  inet::rn::aspect,
  inet::jsonrpc::aspect,
  fas::alias< inet::stream::_incoming_, inet::rn::_input_        >,
  fas::alias< inet::rn::_output_,       inet::stream::_outgoing_ >,
  fas::alias< inet::rn::_incoming_,     inet::jsonrpc::_input_   >,
  fas::alias< inet::jsonrpc::_output_,  inet::rn::_outgoing_     >
>{};

}}
