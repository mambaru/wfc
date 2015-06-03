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

  // iinterface
  virtual void startup_io(io_id_t io_id, outgoing_handler_t handler);
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler);
  virtual void shutdown_io(io_id_t io_id);

private:
  std::shared_ptr<imodule> _impl;
};

}
