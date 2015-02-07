//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/wfc.hpp>
#include <wfc/core/imodule.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/global.hpp>
#include <iostream>
#include "wfc_build_info.h"

namespace wfc{

wfc::wfc(std::string program_version, modules_list modules )
  : _program_version(program_version)
{
  _global = std::make_shared<global>(_io_service);
  for (const auto& p: modules)
    _global->registry.set("module", p.first, p.second);
}

int wfc::run(int argc, char* argv[])
{
  global::static_global = _global;
  _global->program_version = _program_version;
  _global->wfc_version = wfc_build_info_string;

  _global->registry.for_each<imodule>("module", [this](const std::string& name, std::shared_ptr<imodule> module)
  {
    module->create(name,  this->_global);
  });

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
    status = core->run(_global);
  }
  
  std::clog << "wfc::run finalize ... " << std::endl;
  
  _global->registry.clear();
  
  global::static_global.reset();
  _global.reset();
  std::clog << "...finalize done!" << std::endl;
  return status;
}

}
