

#include <comet/core/global.hpp>
#include "global_log.hpp"

namespace mamba{ namespace comet{

logstream global_log(const std::string& name, const std::string& ident)
{
  std::shared_ptr<ilogger> logger_ptr = nullptr;
  if (auto g = global::static_global.lock())
  {
    if (auto lr = g->loggers.lock())
    {
      logger_ptr = lr->find(name).lock();
    }
  }
  
  return logstream( 
    name, 
    ident, 
    logger_ptr
  );
}

}}
