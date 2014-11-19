#pragma once

#include <wfc/core/iinterface.hpp>
#include <memory>

namespace wfc{

struct global;
  
struct istartup: iinterface
{
  virtual ~istartup() {}
  virtual bool startup(int argc, char* argv[]) = 0;
};

}
