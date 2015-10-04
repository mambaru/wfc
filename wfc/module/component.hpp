#pragma once

#include <wfc/module/icomponent.hpp>
#include <memory>
#include <string>

namespace wfc{

class component
  : public icomponent
{
public:
  component(std::shared_ptr<icomponent>);
  virtual ~component();
  virtual std::string name() const;
  virtual std::string description() const;
  virtual std::string interface_name() const;

  virtual std::string generate(const std::string& type) const;
  virtual bool parse(const std::string& conf);
  virtual void configure(const std::string& conf, const std::string& arg);
  virtual void create( std::shared_ptr<wfcglobal>);

// only for external control
  virtual void start(const std::string& arg);
  virtual void stop(const std::string& arg);
  
  // iinterface
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override;
  virtual void unreg_io(io_id_t io_id) override;


private:
  std::shared_ptr<icomponent> _impl;
};

}
