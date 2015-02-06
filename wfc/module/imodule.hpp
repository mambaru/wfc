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

struct imodule: iinterface
{
  enum class priority
  {
    none = -1,
    core,
    config,
    logger,
    gateway,
    low,
    normal,
    hight, 
    service
  };
  
  virtual ~imodule(){}
  
  virtual priority startup_priority() const = 0;
  virtual priority shutdown_priority() const = 0;
  
  virtual std::string version() const = 0;
  virtual std::string description() const = 0;
  virtual std::string generate(const std::string& type) const = 0;
  virtual bool parse_config(const std::string& conf) = 0;
  
  virtual void create( const std::string& name, std::shared_ptr<global> g ) = 0;
  virtual void configure(const std::string& conf, const std::string& arg) = 0;
  virtual void initialize(const std::string& arg) = 0;
  virtual void start(const std::string& arg) = 0;
  virtual void stop(const std::string& arg) = 0;
  virtual void shutdown(const std::string& arg) = 0;
};

}
