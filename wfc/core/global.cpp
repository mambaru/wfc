//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#include <wfc/core/global.hpp>
#include <wfc/core/icore.hpp>
#include <iow/memory.hpp>

namespace wfc{

std::shared_ptr<wfcglobal> wfcglobal::static_global;

}

void wfc_abort(std::string message)
{
  if ( auto g = ::wfc::wfcglobal::static_global )
  {
    g->registry.get< ::wfc::icore >("core")->core_abort(message);
  }
}
