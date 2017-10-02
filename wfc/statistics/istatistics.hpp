#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/api/push.hpp>

namespace wfc{

struct istatistics: iinterface
{
  virtual ~istatistics(){}
  virtual void push( statistics::request::push::ptr req, statistics::response::push::handler cb) = 0;
};

}
