#pragma once

#include <wfc/core/ibuild_info.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/module/ipackage.hpp>
#include <wfc/module/imodule.hpp>
#include <wfc/core/wfcglobal.hpp>

#include <memory>
#include <string>
#include <vector>

namespace wfc{

class core_package_for_testing
  : public std::enable_shared_from_this<core_package_for_testing>
  , public ipackage
  , public istartup
  , public icore
{
public:
  // ipackage
  virtual std::shared_ptr<ibuild_info> build_info() override
  {
    return nullptr;
  }

  virtual std::string name() const override
  {
    return "core_package_for_testing";
  }

  virtual std::string description() const override
  {
    return "Core package for testing";
  }

  virtual void create( std::shared_ptr<wfcglobal> g) override
  {
    if ( g->common_workflow == nullptr )
    {
      g->common_workflow = std::make_shared<wflow::workflow>(g->io_context);
    }

    _global = g;
    auto ptarget = this->shared_from_this();
    _global->registry.set_target("core", std::static_pointer_cast<icore>(ptarget), false);
  }

  virtual int show_order() const override
  {
    return 1000;
  }

  std::shared_ptr<wfcglobal> get_global() const
  {
    return _global;
  }

  // only for external control
  virtual std::vector< std::shared_ptr<imodule> > modules() override
  {
    return std::vector< std::shared_ptr<imodule> >();
  }

  virtual void start(const std::string& ) override
  {

  }

  virtual void stop(const std::string&) override
  {

  }

  // istartup
  virtual int startup(int , char* [], std::string ) override
  {
    return 0;
  }

  virtual bool ready_for_run() override
  {
    return true;
  }

  virtual void clean_finalize() override
  {
  }

  // icore
  virtual int run() override
  {
    for (int i = 0; i < _poll_count; i++)
      _global->io_context.poll();
    return 0;
  }

  virtual void core_reconfigure() override
  {

  }

  virtual void core_stop() override
  {

  }

  virtual void core_abort( const std::string& ) override
  {

  }

  void set_poll_count(int poll_count)
  {
    _poll_count = poll_count;
  }

private:
  std::shared_ptr<wfcglobal> _global;
  int _poll_count = 0;
};

}
