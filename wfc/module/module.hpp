#pragma once

#include <wfc/module/imodule.hpp>
#include <memory>
#include <string>

namespace wfc{

class module
  : public imodule
{
public:
  module(std::shared_ptr<imodule>);
  virtual ~module();
  virtual std::string name() const;
  virtual std::string description() const;
  virtual std::vector< std::shared_ptr<iobject> > objects() const;
  virtual void create( std::shared_ptr<wfcglobal>);
  // only for external control
  virtual void start(const std::string& arg);
  virtual void stop(const std::string& arg);

private:
  std::shared_ptr<imodule> _impl;
};

}
