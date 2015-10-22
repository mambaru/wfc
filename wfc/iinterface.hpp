//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/io/types.hpp>

namespace wfc{

struct iinterface
{
  typedef ::iow::io::data_type data_type;
  typedef ::iow::io::data_ptr  data_ptr;
  typedef ::iow::io::io_id_t   io_id_t;

  typedef ::iow::io::outgoing_handler_t outgoing_handler_t;
  typedef ::iow::io::incoming_handler_t incoming_handler_t;
  typedef ::iow::io::startup_handler_t  startup_handler_t;
  typedef ::iow::io::shutdown_handler_t shutdown_handler_t;

  virtual ~iinterface() {}

  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
  {
    
  }

  virtual void unreg_io(io_id_t /*io_id*/)
  {
    
  }

  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, outgoing_handler_t handler)
  {
    if (handler!=nullptr)
      return handler(nullptr);
  }
};

}
