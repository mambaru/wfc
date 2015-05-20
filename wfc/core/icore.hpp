//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <memory>

#include <wfc/core/iinterface.hpp>

namespace wfc{

struct wfcglobal;
  
struct icore: iinterface
{
  virtual ~icore() {}
  virtual int run( /*std::shared_ptr<wfcglobal> g*/ ) = 0;
  virtual void core_reconfigure() = 0;
  virtual void core_stop() = 0;
};

}
