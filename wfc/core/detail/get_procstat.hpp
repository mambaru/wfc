#pragma once

#include "procstat.hpp"
#include "unistd.h"
#include <vector>

namespace wfc{ namespace detail{

int get_procstat(procstat* pinfo);
int get_proc_threads(std::vector<pid_t> pids);

}}
