//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2007, 2011, 2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include <wfc/system/system.hpp>
#include <wfc/system/unp.hpp>
#include <stdexcept>
#include <ctime>
#include <cstring>
//#include <unistd.h>


namespace wfc{

int error_code()
{
  return errno;
}

std::string strerror(int lasterror)
{
  return std::strerror(lasterror);
}

system_error::system_error(const std::string& msg)
  : std::runtime_error(msg + strerror(error_code()))
{}



int is_atty(int fd)
{
#ifdef HAVE_ISATTY_FUNC
  return ::isatty(fd);
#else
  return false;
#endif
}

bool is_atty_stdout()
{
#ifdef HAVE_ISATTY_FUNC
  return ::isatty( ::fileno( ::stdout));
#else
  return false;
#endif
}

bool is_atty_stderr()
{
#ifdef HAVE_ISATTY_FUNC
  return ::isatty( ::fileno( ::stderr));
#else
  return false;
#endif
}

void sleep( int millisec )
{
#ifdef HAVE_USLEEP_FUNC
  ::usleep(millisec*1000);
#else
  ::sleep(millisec/1000);
#endif
}

void daemonize()
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

void autoup(time_t timeout, std::function<bool(bool, int, time_t)> f )
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
    time_t t = std::time(0);
    ::waitpid(pid, &status, 0);
    time_t worktime = std::time(0) - t;
    bool restart = status!=0 && ( worktime >= timeout );
    if ( f!=nullptr )
    {
      restart = f(restart, status, worktime);
    }
    
    kill(pid, SIGKILL);
    if ( !restart )
      exit(0);
  }
}

int dumpable()
{
#if HAVE_SYS_PRCTL_H
  rlimit core = { RLIM_INFINITY, RLIM_INFINITY };
  return ::prctl(PR_SET_DUMPABLE, 1) || ::setrlimit(RLIMIT_CORE, &core) ? -1 : 0;
#endif
  return -1;
}

}
