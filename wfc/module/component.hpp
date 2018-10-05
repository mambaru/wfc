#pragma once

#include <wfc/module/icomponent.hpp>
#include <memory>
#include <string>

namespace wfc{

class component
  : public icomponent
{
public:
  explicit component(const std::shared_ptr<icomponent>& p);
  virtual ~component();
  
  // icomponent
  virtual std::string name() const override;
  virtual std::string help() const override;
  virtual std::string description() const override;
  virtual std::string interface_name() const override;

  virtual std::string generate(const std::string& type) override;
  virtual bool parse(const std::string& strjson, json::json_error* err) override;
  virtual bool configure(const std::string& strjson, json::json_error* err) override;
  virtual void create( std::shared_ptr<wfcglobal>) override;

// only for external control
  virtual void start(const std::string& arg) override;
  virtual void stop(const std::string& arg) override;
  
  // iinterface
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override;
  virtual void unreg_io(io_id_t io_id) override;
private:
  std::shared_ptr<icomponent> _impl;
};

}
