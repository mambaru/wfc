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

void wfcglobal::clear()
{
  program_build_info = nullptr;
  wfc_build_info = nullptr;
  args.clear();
  idle.clear();
  after_start.clear();
  before_stop.clear();
  after_stop.clear();
  nocall_handler = nullptr;
  doublecall_handler = nullptr;
  cpu.clear();
  registry.clear();
  common_workflow = nullptr;
}

wfcglobal::~wfcglobal()
{
  this->clear();
}

}

