//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <wrtstat/api/push.hpp>
#include <wrtstat/api/multi_push.hpp>
#include <wrtstat/api/del.hpp>

namespace wfc{

struct istatistics: iinterface
{
  typedef wrtstat::request::push::ptr push_ptr;
  typedef wrtstat::request::multi_push::ptr multi_push_ptr;
  typedef wrtstat::request::del::ptr del_ptr;
  
  typedef wrtstat::response::push::handler push_handler;
  typedef wrtstat::response::multi_push::handler multi_push_handler;
  typedef wrtstat::response::del::handler del_handler;

  virtual ~istatistics() = default;
  virtual void push( push_ptr req, push_handler cb) = 0;
  virtual void multi_push( multi_push_ptr req, multi_push_handler cb) = 0;
  virtual void del( del_ptr req, del_handler cb) = 0;
};

}
