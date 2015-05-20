//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2007, 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/system/unp.hpp>
#include <stdexcept>

namespace wfc{

inline int error_code()
{
  return errno;
}

inline std::string strerror(int lasterror)
{
  return ::strerror(lasterror);
}

struct system_error
  : public std::runtime_error
{
  explicit system_error(const std::string& msg)
    : std::runtime_error(msg + strerror(error_code()))
  {
  }
};

  
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


inline void daemonize()
{
  int null = ::open("/dev/null", O_RDWR);
  if(-1 == null)
  {
    ::perror("/dev/null");
    ::exit(EXIT_FAILURE);
  }

  switch(::fork())
  {
  case 0:
    ::setsid();
    ::umask(0);
    ::close(0);
    ::close(1);
    ::close(2);
    ::dup2(null, 0);
    ::dup2(null, 1);
    ::dup2(null, 2);
    break;

  case -1:
    ::perror("fork()");
    ::exit(EXIT_FAILURE);

  default:
    ::exit(EXIT_SUCCESS);
  }
}

inline void autoup(time_t timeout, std::function<void(bool, int)> f = nullptr)
{
  for (;;)
  {
    pid_t pid = ::fork();
    
    if (pid==0)
    {
      int null = ::open("/dev/null", O_RDWR);
      if( -1 != null)
      {
        ::close(0);
        ::close(1);
        ::close(2);
        ::dup2(null, 0);
        ::dup2(null, 1);
        ::dup2(null, 2);
      }
      break;
    }
    
    int status = 0;
    time_t t = time(0);
    ::waitpid(pid, &status, 0);    
    bool restart = status!=0 && ( time(0) - t >= timeout );
    f(restart, status);
    kill(pid, SIGKILL);
    if ( !restart )
      exit(0);
  }
}

inline int dumpable()
{
#if HAVE_SYS_PRCTL_H
  rlimit core = { RLIM_INFINITY, RLIM_INFINITY };
  return ::prctl(PR_SET_DUMPABLE, 1) || ::setrlimit(RLIMIT_CORE, &core) ? -1 : 0;
#endif
  return -1;
}


}
