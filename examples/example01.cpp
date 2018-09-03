#include <wfc/wfc.hpp>

int main(int argc, char* argv[])
{
  wfc::wfc< wfc::empty_build_info >({}).run(argc, argv);
}
