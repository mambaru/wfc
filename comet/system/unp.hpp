//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2007, 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <comet/system/config.h>

#include <cassert>
#include <cctype> 
#include <cerrno> 
#include <cfloat> 
#include <climits> 
#include <clocale> 
#include <cmath> 
#include <csetjmp> 
#include <csignal> 
#include <cstdarg> 
#include <cstdbool> 
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>

/* Define to 1 if you have the <sys/time.h> header file. */
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_RESORCE_H
#include <sys/resource.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* Define to 1 if you have the <sys/timeb.h> header file. */
#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

