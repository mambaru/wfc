//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

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

  typedef ::wjrpc::outgoing_handler_t outgoing_handler_t;
  typedef ::wjrpc::incoming_handler_t incoming_handler_t;

  virtual ~ijsonrpc() = default;
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) = 0;
  virtual void perform_outgoing(outgoing_holder, io_id_t /*id*/) = 0;
};

}
