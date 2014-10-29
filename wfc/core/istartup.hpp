#pragma once

#include <memory>

namespace wfc{

struct global;
  
struct istartup
{
  virtual ~istartup() {}
  virtual bool startup(int argc, char* argv[]) = 0;
};

}
