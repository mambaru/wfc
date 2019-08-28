//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/api/push.hpp>
#include <wfc/statistics/api/del.hpp>

namespace wfc{

struct istatistics: iinterface
{
  virtual ~istatistics() = default;
  virtual void push( statistics::request::push::ptr req, statistics::response::push::handler cb) = 0;
  virtual void del( statistics::request::del::ptr req, statistics::response::del::handler cb) = 0;
};

}
