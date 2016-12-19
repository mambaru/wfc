#pragma once

#include "procstat.hpp"
#include "unistd.h"

namespace wfc{ namespace detail{

int get_procstat(procstat* pinfo);


}}
