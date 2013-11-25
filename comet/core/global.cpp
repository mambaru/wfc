#include <comet/core/global.hpp>
#include <comet/core/ilogger.hpp>
#include <memory>

namespace mamba{ namespace comet{

std::weak_ptr<ilogger> global::global_logger = std::weak_ptr<ilogger>();

}}
