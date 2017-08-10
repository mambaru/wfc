#pragma once

#include <wjrpc/types.hpp>
#include <wfc/iinterface.hpp>

namespace wfc{

struct ijsonrpc
  : iinterface
{
  typedef ::wjrpc::incoming_holder incoming_holder;
  typedef ::wjrpc::outgoing_holder outgoing_holder;

  typedef iinterface::output_handler_t output_handler_t;
  typedef iinterface::input_handler_t input_handler_t;
  typedef iinterface::startup_handler_t  io_startup_handler_t;
  typedef iinterface::shutdown_handler_t io_shutdown_handler_t;

  typedef ::wjrpc::outgoing_handler_t rpc_outgoing_handler_t;
  typedef ::wjrpc::incoming_handler_t rpc_incoming_handler_t;
  typedef ::wjrpc::startup_handler_t  rpc_startup_handler_t;
  typedef ::wjrpc::shutdown_handler_t rpc_shutdown_handler_t;

  virtual ~ijsonrpc(){}
  virtual void perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t handler) = 0;
  virtual void perform_outgoing(outgoing_holder, io_id_t /*id*/) = 0;
};

}
