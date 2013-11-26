#pragma once

#include <sstream>
#include <string>

namespace mamba{ namespace comet{
  
struct ilogger
{
  virtual ~ilogger(){}
  //virtual void initialize(const std::string& name, std::stringstream& str) = 0;
  //virtual void write(const std::string& name, const std::string& ident,  const std::string& str) = 0;

  virtual void initialize(const std::string& name, std::stringstream& str) = 0;
  virtual void write(const std::string& name, const std::string& ident,  const std::string& str) = 0;
};

}}
