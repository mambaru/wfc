#include <wfc/wfc.hpp>
#include <wfc/module/ipackage.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/build_info.hpp>
#include <wfc/core/wfcglobal.hpp>
#include <memory>
#include <iostream>

namespace {

class example_startup
  : public wfc::istartup
{
public:
  virtual int startup(int argc, char* argv[], std::string helpstring) override
  {
    std::cout << "example_startup::startup: " << helpstring << std::endl;
    if ( argc > 1 )
    {
      if ( std::string(argv[1]) == std::string("run"))
        _ready = true;
      else
        return 1;
    }
    return 0;
  }

  virtual bool ready_for_run() override
  {
    return _ready;
  }


private:
  bool _ready = false;
};

class example_core
  : public wfc::icore
{
  virtual int run( ) override
  {
    std::cout << "example_core::run()" << std::endl;
    this->core_reconfigure();
    return 0;
  }

  virtual void core_reconfigure() override{ }

  virtual void core_stop() override{}

  virtual void core_abort( const std::string& ) override {}

  virtual void core_restart() override {}

  virtual void set_status(wfc::core_status, const std::string&) override{}

  virtual void set_stage(wfc::core_stage) override{}

  virtual wfc::core_status get_status(wfc::core_stage*, std::vector<std::pair<wfc::core_status, std::string>>* ) override
  {
    return wfc::core_status::OK;
  }

  virtual std::string get_status_text(size_t, size_t) override
  {
    return "OK";
  }

};



class example_package
  : public wfc::ipackage
{
public:
  virtual std::shared_ptr<wfc::ibuild_info> build_info() override { return nullptr; }

  virtual std::string name() const override
  {
    std::cout << "example_package::name()" << std::endl;
    return "example_package";
  }

  virtual std::string description() const override { return std::string(); }

  virtual int show_order() const override { return 0; }

  virtual std::vector< std::shared_ptr<wfc::imodule> > modules() override
  {
    return std::vector< std::shared_ptr<wfc::imodule> >();
  }

  virtual void create( std::shared_ptr<wfc::wfcglobal> g) override
  {
    g->registry.set_target("core", std::make_shared<example_core>());
    g->registry.set_target("startup", std::make_shared<example_startup>());
    std::cout << "example_package::create()" << std::endl;
  }

  // only for external control
  virtual void start(const std::string&) override { }

  virtual void stop(const std::string&) override  { }
};

}

int main(int argc, char* argv[])
{
  wfc::wfc< wfc::empty_build_info >({
    std::make_shared<example_package>()
  }).run(argc, argv);
}
