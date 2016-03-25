//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#include <wfc/core/global.hpp>
#include <wfc/core/icore.hpp>
#include <iow/memory.hpp>

void wfc_exit()
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    g->registry.get< ::wfc::icore >("core")->core_stop();
  }
}

void wfc_exit_with_error(std::string message)
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    g->registry.get< ::wfc::icore >("core")->core_abort(message);
  }
}

void wfc_restart()
{
  wfc_exit();
}

void wfc_restart_with_error(std::string message)
{
  wfc_exit_with_error(message);
}
