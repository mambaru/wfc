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
#include <wfc/core/wfcglobal.hpp>
#include <iostream>

#include "wfc_build_info.h"
namespace wfc{

basic_wfc::~basic_wfc()
{
  if ( _global != nullptr )
    _global->registry.clear();
  _packages.clear();
  _global.reset();
}

basic_wfc::basic_wfc(std::shared_ptr<ibuild_info> bi, const package_list& packages )
{
  _global = std::make_shared<wfcglobal>(_io_context);
  _global->program_build_info = bi;
  _global->wfc_build_info = make_build_info<wfc_build_info>();
  this->add_packages(packages);
}

void basic_wfc::add_package(package_ptr ppack)
{
  _packages.push_back(ppack);
  _global->registry.set_object("package", ppack->name(), ppack);
  ppack->create(_global);
}

void basic_wfc::add_packages(const package_list& packages)
{
  for (const auto& p: packages)
    this->add_package(p);
}


int basic_wfc::run(int argc, char* argv[], std::string helpstring)
{
  wfcglobal::static_global = _global;

  if ( auto startup = _global->registry.get_target<istartup>("startup", true) )
  {

    if ( int err = startup->startup(argc, argv, helpstring) )
      return err;

    if ( !startup->ready_for_run() )
      return 0;
  }

  int status = 0;

  if ( auto core = _global->registry.get_target<icore>("core", true) )
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
