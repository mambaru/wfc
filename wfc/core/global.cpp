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

wfcglobal::wfcglobal( io_service_type& io_service)
  : io_service(io_service)
  , workflow( new ::wfc::workflow(io_service) )
{
  
}

}

