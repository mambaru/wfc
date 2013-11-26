#include <comet/core/global.hpp>
#include <comet/logger/ilogger.hpp>
#include <memory>

namespace mamba{ namespace comet{

std::weak_ptr<global> global::static_global;
  /*
std::weak_ptr<ilogger> global::global_logger= std::weak_ptr<ilogger>();
std::weak_ptr<icore>   global::global_core   = std::weak_ptr<icore>();
*/
}}
