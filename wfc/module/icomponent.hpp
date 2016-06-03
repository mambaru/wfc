//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <memory>
#include <string>

namespace wfc{

struct wfcglobal;

// Multiton and singleton
// 
struct icomponent: iinterface
{
  virtual ~icomponent(){}

  virtual std::string name()  = 0;
  virtual std::string description()  = 0;
  virtual std::string interface_name()  = 0;

  virtual std::string generate(const std::string& type)  = 0;
  virtual bool parse(const std::string& conf) = 0;

  virtual void create( std::shared_ptr<wfcglobal>) = 0;
  virtual void configure(const std::string& conf, const std::string& arg)  = 0;

// only for external control
  virtual void start(const std::string& arg) = 0;
  virtual void stop(const std::string& arg) = 0;
};

}
