#include <wfc/wfc.hpp>
#include <wfc/module/module_list.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/build_info.hpp>

#include <memory>
#include <iostream>

namespace {
struct example_package: 
  wfc::module_list<
    wfc::empty_build_info
  >
{
  virtual std::string name() const override
  {
    return "example_package";
  }
};
}

int main(int argc, char* argv[])
{
  wfc::wfc< wfc::empty_build_info >({
    std::make_shared<example_package>()
  }).run(argc, argv);
}
