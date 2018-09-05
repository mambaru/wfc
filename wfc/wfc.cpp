//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/wfc.hpp>
#include <wfc/module/ipackage.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/core/istartup.hpp>
#include <wfc/core/build_info.hpp>
#include <wfc/core/global.hpp>
#include <iostream>
#include "wfc_build_info.h"

namespace wfc{

basic_wfc::basic_wfc(std::shared_ptr<ibuild_info> bi, const package_list& packages )
  : _packages(packages)
{
  _global = std::make_shared<wfcglobal>(_io_service);
  _global->program_build_info = bi;
  _global->wfc_build_info = make_build_info<wfc_build_info>();

  for (const auto& p: packages)
  {
    _global->registry.set("package", p->name(), p);
  }

  for (const auto& p: packages)
  {
    p->create(_global);
  }

}

int basic_wfc::run(int argc, char* argv[], std::string helpstring)
{
  wfcglobal::static_global = _global;

  if ( auto startup = _global->registry.get<istartup>("startup", true) )
  {
    if ( int err = startup->startup(argc, argv, helpstring) )
      return err;
    
    if ( !startup->ready_for_run() )
      return 0;
  }

  int status = 0;

  if ( auto core = _global->registry.get<icore>("core", true) )
  {
    status = core->run();
  }
  else
  {
    return 2;
  }

  std::clog << "wfc::run finalize with status=" << status << "..." << std::endl;

  _global->registry.clear();

  wfcglobal::static_global.reset();
  _global.reset();
  std::clog << "...finalize done!" << std::endl;
  return status;
}

}
