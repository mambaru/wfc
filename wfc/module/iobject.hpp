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

struct wfcglobal;

// Multiton and singleton
// 
struct iobject: iinterface
{
  virtual ~iobject(){}

  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual std::string interface_name() const = 0;

  virtual std::string generate(const std::string& type) const = 0;
  virtual void parse(const std::string& conf) = 0;

  virtual void create( std::shared_ptr<wfcglobal>) = 0;
  virtual void configure(const std::string& conf, const std::string& arg)  = 0;

// only for external control
  virtual void start(const std::string& arg) = 0;
  virtual void stop(const std::string& arg) = 0;
};

}
