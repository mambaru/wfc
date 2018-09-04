#include <wfc/wfc.hpp>
#include "core_package/core_package.hpp"

int main(int argc, char* argv[])
{
  wfc::wfc< wfc::empty_build_info >({
    std::make_shared<core_package>()
  }).run(argc, argv, "example7");
}
