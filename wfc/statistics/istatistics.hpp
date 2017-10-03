#pragma once

#include <wfc/iinterface.hpp>
#include <wfc/statistics/api/push.hpp>
#include <wfc/statistics/api/del.hpp>

namespace wfc{

struct istatistics: iinterface
{
  virtual ~istatistics(){}
  virtual void push( statistics::request::push::ptr req, statistics::response::push::handler cb) = 0;
  virtual void del( statistics::request::del::ptr req, statistics::response::del::handler cb) = 0;
};

}
