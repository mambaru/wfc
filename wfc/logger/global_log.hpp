#pragma once

#include <wfc/logger/logstream.hpp>
#include <string>
#include <sstream>

namespace wfc{

#ifndef WFC_DISABLE_GLOBAL_LOG
  logstream global_log(const std::string& name, const std::string& ident);
#else
  logstream global_log(const std::string& name, const std::string& ident)
  {
    return logstream(name, ident, nullptr);
  }
#endif
}
