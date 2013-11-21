//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2007, 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <comet/system/unp.hpp>

namespace mamba{ namespace comet{

inline int isatty(int fd)
{
#ifdef HAVE_ISATTY_FUNC
  return ::isatty(fd);
#else
  return false;
#endif
}

inline bool is_atty_stdout()
{
#ifdef HAVE_ISATTY_FUNC
  return ::isatty( ::fileno( ::stdout));
#else
  return false;
#endif
}

inline bool is_atty_stderr()
{
#ifdef HAVE_ISATTY_FUNC
  return ::isatty( ::fileno( ::stderr));
#else
  return false;
#endif
}


inline void sleep( int millisec )
{
#ifdef HAVE_USLEEP_FUNC
  ::usleep(millisec*1000);
#else
  ::sleep(millisec/1000);
#endif
}


}}
