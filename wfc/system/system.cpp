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

#include <iostream>
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

std::function<void()> daemonize(bool wait)
{
  int null = ::open("/dev/null", O_RDWR);
  if(-1 == null)
  {
    ::perror("/dev/null");
    ::exit(EXIT_FAILURE);
  }

  if (wait)
    ::signal(SIGTERM, [](int){ ::exit(EXIT_SUCCESS); });
  pid_t pid = ::fork();
  switch(pid)
  {
  case 0:
    if (wait)
      ::signal(SIGTERM, SIG_DFL);
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
    // Wait SIGTERM from children for exit
    int status = EXIT_SUCCESS;
    if (wait) ::wait(&status);
    ::exit(status);
    return nullptr;
  }
  
  pid_t ppid = ::getppid();
  return [ppid, wait]()
  {
    if ( wait )
      ::kill( ppid, SIGTERM);
  };
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

bool change_user(std::string username)
{
  struct passwd *pwd = ::getpwnam(username.c_str() ); /* don't free, see getpwnam() for details */
  if( pwd == NULL )
  {
    std::cerr << strerror(errno) << std::endl;
    return false; 
  }
  uid_t uid = pwd->pw_uid;
  std::clog << "new uid=" << uid << std::endl;
  if ( 0 != ::setuid(uid) )
  {
    std::cerr << strerror(errno) << std::endl;
    return false; 
  };
  return true;
}

bool change_working_directory(std::string working_directory)
{
  if ( 0 != ::chdir( working_directory.c_str() ) )
  {
    std::cerr << strerror(errno) << std::endl;
    return false; 
  }
  return true;
}


}
