#pragma once

#include <memory>

namespace wfc{

struct global;
  
struct istartup
{
  virtual ~istartup() {}
  virtual bool startup(int argc, char* argv[] /*std::weak_ptr<global> g*/) = 0;
};

}
