#pragma once

#include "imodule.hpp"

namespace wfc{
  
class module
  : public ::wfc::imodule
{
public:
  /*module();*/

  module( std::shared_ptr< ::wfc::imodule > impl);

  virtual priority startup_priority() const;
  
  virtual priority shutdown_priority() const;
  
  virtual std::string version() const;
  
  virtual std::string description() const;
  
  virtual std::string generate(const std::string& type) const;

  virtual bool parse_config(const std::string& confstr);
  
  virtual void create( const std::string& name, std::shared_ptr< ::wfc::global> g );
  
  virtual void configure(const std::string& conf, const std::string& args);
  
  virtual void initialize(const std::string& args);
  
  virtual void start(const std::string& args);
  
  virtual void stop(const std::string& args);
  
  virtual void shutdown(const std::string& args);

private:

  std::shared_ptr< ::wfc::imodule > _impl;

};

}
