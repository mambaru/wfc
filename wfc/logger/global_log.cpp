#include <wfc/core/global.hpp>
#include "global_log.hpp"

namespace wfc{

logstream global_log(const std::string& name, const std::string& ident, const std::string& logger)
{
  std::shared_ptr<ilogger> logger_ptr = nullptr;
  if (auto g = global::static_global)
  {
    logger_ptr = g->registry.get<ilogger>(logger);
    /*if (auto lr = g->loggers)
    {
      logger_ptr = lr->get(name);
    }
    */
  }
  
  return logstream( 
    name, 
    ident, 
    logger_ptr
  );
}

}
