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
  virtual std::string name() const override;
  virtual std::string description() const override;
  virtual std::vector< std::shared_ptr<iobject> > objects() const override;
  virtual void create( std::shared_ptr<wfcglobal>) override;
  // only for external control
  virtual void start(const std::string& arg) override;
  virtual void stop(const std::string& arg) override;

  // iinterface
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override;
  virtual void unreg_io(io_id_t io_id) override;

private:
  std::shared_ptr<imodule> _impl;
};

}
