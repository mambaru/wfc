//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/imodule.hpp>
#include <wfc/module/icomponent.hpp>
#include <memory>
#include <string>

namespace wfc{

class module
  : public imodule
{
public:
  explicit module(const std::shared_ptr<imodule>& p);
  virtual ~module();
  virtual std::string name() const override;
  virtual std::string description() const override;
  virtual std::vector< std::shared_ptr<icomponent> > components() const override;
  virtual void create( std::shared_ptr<wfcglobal>) override;
  // only for external control
  virtual void start(const std::string& arg) override;
  virtual void stop(const std::string& arg) override;

  // iinterface
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override;
  virtual void unreg_io(io_id_t io_id) override;

private:
  std::shared_ptr<imodule> _impl;
};

}
