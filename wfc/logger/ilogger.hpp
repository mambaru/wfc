#pragma once

#include <wfc/iinterface.hpp>
#include <wlog/logger_fun.hpp>

#include <string>

namespace wfc{
  
struct ilogger: iinterface
{
  virtual ~ilogger(){}
  typedef wlog::formatter_fun formatter_t;
  virtual formatter_t formatter() = 0;
};

}
