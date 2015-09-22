#pragma once

#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>
#include <wfc/iinterface.hpp>

namespace wfc{

struct ijsonrpc
  : iinterface
{
  typedef ::iow::jsonrpc::incoming_holder incoming_holder;
  typedef ::iow::jsonrpc::outgoing_holder outgoing_holder;

  virtual ~ijsonrpc(){}
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) = 0;
  virtual void perform_outgoing(outgoing_holder, io_id_t /*id*/) = 0;
};

}
