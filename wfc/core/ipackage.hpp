//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/core/iinterface.hpp>
#include <memory>
#include <string>

namespace wfc{

struct imodule;
struct wfcglobal;

// Убрать нафиг - это модуль
struct ipackage: iinterface
{
  virtual ~ipackage() {}
  virtual std::string version() const= 0;
  virtual std::string description() const = 0;
  virtual std::vector< std::shared_ptr<imodule> > modules() const = 0;
  virtual void create( std::shared_ptr<wfcglobal> ) = 0;
};

}
