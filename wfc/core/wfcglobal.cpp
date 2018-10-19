#include <wfc/core/wfcglobal.hpp>
#include <wfc/core/icore.hpp>
#include <iow/memory.hpp>

namespace wfc{

std::shared_ptr<wfcglobal> wfcglobal::static_global;

wfcglobal::wfcglobal( io_service_type& ios)
  : io_service(ios)
{
  stop_signal_flag = false;
}


}

