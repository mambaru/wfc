#pragma once

#include <wfc/logger/logstream.hpp>
#include <string>

namespace wfc{

logstream global_log(const std::string& name, const std::string& ident);

}
