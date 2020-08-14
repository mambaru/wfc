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

void wfc_abort(const std::string& message)
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    if (auto c = g->registry.get_target< ::wfc::icore >("core", true) )
      c->core_abort(message);
  }
}

void wfc_restart()
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    if (auto c = g->registry.get_target< ::wfc::icore >("core", true) )
      c->core_restart();
  }
}

