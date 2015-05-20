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

struct global;

struct iinstance: iinterface
{
  virtual ~iinstance(){}
  
  virtual std::string name() const = 0;
  virtual int startup_priority() const = 0;
  virtual int shutdown_priority() const = 0;

  virtual void initialize(/*const std::string& arg*/) = 0;
  virtual void start(const std::string& arg) = 0;
  virtual void stop(const std::string& arg) = 0;
  virtual void perform(const std::string& arg) = 0;
};

}
