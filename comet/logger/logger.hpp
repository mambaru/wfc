#pragma once

#include <comet/logger/logstream.hpp>
#include <comet/logger/global_log.hpp>
#include <memory>

namespace mamba{ namespace comet{

logstream logger(const std::string& name, const std::string& ident)
{
  return logstream( 
    name, 
    ident, 
    global_log::instance().find(name)  
  );
}

}}
