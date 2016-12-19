#pragma once

#include "procstat.hpp"
#include "unistd.h"
#include <vector>

namespace wfc{ namespace detail{

int get_procstat(procstat* pinfo);
int get_threadstat(pid_t pid, procstat* pinfo);
int get_pids_threads(std::vector<pid_t>& pids);

}}
