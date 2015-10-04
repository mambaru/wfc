#pragma once

#include <iow/jsonrpc/types.hpp>
#include <wfc/iinterface.hpp>

namespace wfc{

struct ijsonrpc
  : iinterface
{
  typedef ::iow::jsonrpc::incoming_holder incoming_holder;
  typedef ::iow::jsonrpc::outgoing_holder outgoing_holder;

  typedef iinterface::outgoing_handler_t io_outgoing_handler_t;
  typedef iinterface::incoming_handler_t io_incoming_handler_t;
  typedef iinterface::startup_handler_t  io_startup_handler_t;
  typedef iinterface::shutdown_handler_t io_shutdown_handler_t;

  typedef ::iow::jsonrpc::outgoing_handler_t rpc_outgoing_handler_t;
  typedef ::iow::jsonrpc::incoming_handler_t rpc_incoming_handler_t;
  typedef ::iow::jsonrpc::startup_handler_t  rpc_startup_handler_t;
  typedef ::iow::jsonrpc::shutdown_handler_t rpc_shutdown_handler_t;

  virtual ~ijsonrpc(){}
  virtual void perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t handler) = 0;
  virtual void perform_outgoing(outgoing_holder, io_id_t /*id*/) = 0;
};

}
