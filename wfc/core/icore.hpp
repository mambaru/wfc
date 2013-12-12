#pragma once

#include <memory>

namespace wfc{

struct global;
  
struct icore
{
  virtual ~icore() {}
  virtual int run( int argc, char* argv[], std::weak_ptr<global> g ) = 0;
  virtual void reconfigure() = 0;
  virtual void stop() = 0;
};

}
