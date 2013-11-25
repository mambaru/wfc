#pragma once

#include <comet/core/global.hpp>
#include <comet/logger/logstream.hpp>
#include <memory>

namespace mamba{ namespace comet{

logstream glogger(const std::string& name, const std::string& ident)
{
  return logstream( 
    name, 
    ident, 
    global::global_logger.lock()
  );
}

}}
