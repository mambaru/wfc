#include <wfc/core/global.hpp>
#include "global_log.hpp"

namespace wfc{

logstream global_log(const std::string& name, const std::string& ident)
{
  std::shared_ptr<ilogger> logger_ptr = nullptr;
  if (auto g = global::static_global)
  {
    if (auto lr = g->loggers)
    {
      logger_ptr = lr->get(name);
    }
  }
  
  return logstream( 
    name, 
    ident, 
    logger_ptr
  );
}

}
