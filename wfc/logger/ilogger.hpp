#pragma once

#include <wfc/core/iinterface.hpp>

#include <sstream>
#include <string>

namespace wfc{
  
struct ilogger: iinterface
{
  virtual ~ilogger(){}
  //virtual void initialize(const std::string& name, std::stringstream& str) = 0;
  //virtual void write(const std::string& name, const std::string& ident,  const std::string& str) = 0;

  virtual void initialize(const std::string& name, std::stringstream& str) = 0;
  virtual void write(const std::string& name, const std::string& ident,  const std::string& str) = 0;
};

}
