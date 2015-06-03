#pragma once

#include <wfc/module/iobject.hpp>
#include <memory>
#include <string>

namespace wfc{

class object
  : public iobject
{
public:
  object(std::shared_ptr<iobject>);
  virtual ~object();
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
  virtual void startup_io(io_id_t io_id, outgoing_handler_t handler) override;
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override;
  virtual void shutdown_io(io_id_t io_id) override;


private:
  std::shared_ptr<iobject> _impl;
};

}
