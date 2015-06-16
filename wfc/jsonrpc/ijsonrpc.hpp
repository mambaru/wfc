#pragma once 

#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/iinterface.hpp>

namespace wfc{ namespace jsonrpc{

struct ijsonrpc
  : iinterface
{
  typedef ::iow::jsonrpc::incoming_holder incoming_holder;
  virtual ~ijsonrpc(){}
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) = 0;
};

}}
