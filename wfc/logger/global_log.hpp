#pragma once

#include <wfc/logger/logstream.hpp>
#include <string>
#include <sstream>

namespace wfc{

#ifndef WFC_DISABLE_GLOBAL_LOG
  logstream global_log(const std::string& name, const std::string& ident, const std::string& logger);
#else
  logstream global_log(const std::string& name, const std::string& ident, const std::string& )
  {
    return logstream(name, ident, nullptr);
  }
#endif
}
