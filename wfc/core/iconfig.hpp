#pragma once

#include <wfc/core/iinterface.hpp>
#include <string>

namespace wfc{

struct iconfig: iinterface
{
  virtual ~iconfig() {}
  virtual void reconfigure() = 0;
  virtual void initialize(std::string path) = 0;
  virtual std::string get_config(std::string name) = 0;
  virtual std::string generate(std::string type, std::string path) = 0;
};

}
