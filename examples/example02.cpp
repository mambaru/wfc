#include <wfc/wfc.hpp>
#include <wfc/module/ipackage.hpp>
#include <wfc/core/build_info.hpp>
#include <memory>
#include <iostream>

namespace {
class example_package
  : public wfc::ipackage
{
public:
  example_package() {}

  virtual std::shared_ptr<wfc::ibuild_info> build_info() override
  {
    std::cout << "example_package::build_info()" << std::endl;
    return nullptr;
  }

  virtual std::string name() const override
  {
    std::cout << "example_package::name()" << std::endl;
    return "example_package";
  }

  virtual std::string description() const override
  {
    std::cout << "example_package::description()" << std::endl;
    return "empty package";
  }

  virtual int show_order() const override
  {
    std::cout << "example_package::show_order()" << std::endl;
    return 0;
  }

  virtual std::vector< std::shared_ptr<wfc::imodule> > modules() override
  {
    std::cout << "example_package::modules()" << std::endl;
    return std::vector< std::shared_ptr<wfc::imodule> >();
  }

  virtual void create( std::shared_ptr<wfc::wfcglobal> ) override
  {
    std::cout << "example_package::create()" << std::endl;
  }

  // only for external control
  virtual void start(const std::string&) override
  {
    std::cout << "example_package::start()" << std::endl;
  }

  virtual void stop(const std::string&) override
  {
    std::cout << "example_package::stop()" << std::endl;
  }
};
}

int main(int argc, char* argv[])
{
  wfc::wfc< wfc::empty_build_info >({
    std::make_shared<example_package>()
  }).run(argc, argv);
}
