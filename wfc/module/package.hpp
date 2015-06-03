#pragma once

#include <wfc/module/ipackage.hpp>
#include <memory>
#include <string>

namespace wfc{

class package
  : public ipackage
{
public:
  virtual ~package();
  package(std::shared_ptr<ipackage>);
  virtual std::shared_ptr<ibuild_info> build_info() const;
  virtual std::string description() const;
  virtual std::vector< std::shared_ptr<imodule> > modules() const ;
  virtual void create( std::shared_ptr<wfcglobal> );
  virtual void start(const std::string& arg);
  virtual void stop(const std::string& arg);
  
  // iinterface
  virtual void startup_io(io_id_t io_id, outgoing_handler_t handler);
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler);
  virtual void shutdown_io(io_id_t io_id);

private:
  std::shared_ptr<ipackage> _impl;
};

}
