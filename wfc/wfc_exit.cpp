//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#include <wfc/core/wfcglobal.hpp>
#include <wfc/core/icore.hpp>
#include <iow/memory.hpp>

void wfc_exit()
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    if (auto c = g->registry.get_target< ::wfc::icore >("core", true) )
      c->core_stop();
  }
}

void wfc_exit_with_error(const std::string& message)
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    if (auto c = g->registry.get_target< ::wfc::icore >("core", true) )
      c->core_abort(message);
  }
}

void wfc_restart()
{
  wfc_exit();
}

void wfc_restart_with_error(const std::string& message)
{
  wfc_exit_with_error(message);
}
