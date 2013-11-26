#pragma once

#include <comet/logger/logstream.hpp>
#include <string>

namespace mamba{ namespace comet{

logstream global_log(const std::string& name, const std::string& ident);

}}
