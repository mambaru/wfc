#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/api/add.hpp>

namespace wfc{

struct ibtp: iinterface
{
  virtual ~ibtp(){}
  virtual void add( btp::request::add::ptr req, btp::response::add::handler cb) = 0;
};

}
