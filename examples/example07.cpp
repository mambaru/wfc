#include <wfc/wfc.hpp>
#include "example_package/example_package.hpp"
#include "example07_build_info.h"
int main(int argc, char* argv[])
{
  wfc::wfc< example07_build_info >({
    std::make_shared<example_package>()
  }).run(argc, argv, "example7");
}
