#pragma once

#include <memory>

#include <wfc/core/iinterface.hpp>

namespace wfc{

struct global;
  
struct icore: iinterface
{
  virtual ~icore() {}
  virtual int run( std::shared_ptr<global> g ) = 0;
  virtual void reconfigure() = 0;
  virtual void stop() = 0;
};

}
