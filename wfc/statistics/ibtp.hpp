#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/api/push.hpp>

namespace wfc{

struct ibtp: iinterface
{
  virtual ~ibtp(){}
  virtual void push( btp::request::push::ptr req, btp::response::push::handler cb) = 0;
};

}
