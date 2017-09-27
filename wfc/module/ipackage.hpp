//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <memory>
#include <string>
#include <vector>

namespace wfc{

struct imodule;
struct ibuild_info;
struct wfcglobal;

struct ipackage: iinterface
{
  virtual ~ipackage() {}
  virtual std::shared_ptr<ibuild_info> build_info() = 0;
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual int order() const = 0;
  virtual std::vector< std::shared_ptr<imodule> > modules() = 0;
  virtual void create( std::shared_ptr<wfcglobal> ) = 0;
  // only for external control
  virtual void start(const std::string& arg) = 0;
  virtual void stop(const std::string& arg) = 0;
  
  

};

}
