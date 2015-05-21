//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/wfc.hpp>
#include <wfc/module/ipackage.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/global.hpp>
#include <iostream>
#include "wfc_build_info.h"

namespace wfc{

wfc::wfc(std::string program_version, package_list packages )
  : _program_version(program_version)
  , _packages(packages)
{
  _global = std::make_shared<wfcglobal>(_io_service);

  for (const auto& p: packages)
  {
    _global->registry.set("package", p);
  }

  for (const auto& p: packages)
  {
    p->create(_global);
  }

  _global->program_version = _program_version;
  _global->wfc_version = wfc_build_info_string;
}

int wfc::run(int argc, char* argv[])
{
  wfcglobal::static_global = _global;

  if ( auto startup = _global->registry.get<istartup>("startup") )
  {
    if ( !startup->startup(argc, argv) )
      return 0;
  }
  else
  {
    std::cerr << "no startup module" << std::endl;
  }

  int status = 0;

  if ( auto core = _global->registry.get<icore>("core") )
  {
    status = core->run();
  }

  std::clog << "wfc::run finalize ... " << std::endl;

  _global->registry.clear();

  wfcglobal::static_global.reset();
  _global.reset();
  std::clog << "...finalize done!" << std::endl;
  return status;
}

}
