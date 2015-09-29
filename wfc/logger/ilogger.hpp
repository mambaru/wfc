#pragma once

#include <wfc/iinterface.hpp>

#include <string>

namespace wfc{
  
struct ilogger: iinterface
{
  virtual ~ilogger(){}
  virtual void write(const std::string& name, const std::string& ident, std::string str) = 0;
};

}
