//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <memory>

#include <wfc/iinterface.hpp>

namespace wfc{

struct wfcglobal;
  
struct icore: iinterface
{
  virtual ~icore() {}
  virtual int run() = 0;
  virtual void core_reconfigure() = 0;
  virtual void core_stop() = 0;
  virtual void core_abort( std::string message ) = 0;
};

}
