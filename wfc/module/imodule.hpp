//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/iinterface.hpp>
#include <wfc/module/iobject.hpp>
#include <memory>
#include <string>
#include <vector>

struct wfcglobal;

namespace wfc{

struct imodule: iinterface
{
  virtual ~imodule(){}
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual std::vector< std::shared_ptr<iobject> > objects() const = 0;
  virtual void create( std::shared_ptr<wfcglobal>) = 0;

  // only for external control
  virtual void start(const std::string& arg) = 0;
  virtual void stop(const std::string& arg) = 0;
};

}
