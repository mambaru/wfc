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
#include <iostream>
#include <atomic>
//#include <sys/prctl.h>

namespace wfc{

template<typename Args>
inline void unused_params(const Args& ...) {}


int is_atty(int fd)
{
#ifdef HAVE_ISATTY_FUNC
  return ::isatty(fd);
#else
  unused_params(fd);
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
  ::usleep( static_cast<useconds_t>(millisec*1000));
#else
  ::sleep(millisec/1000);
#endif
}

std::function<void()> daemonize(bool wait_flag)
{
  int null = ::open("/dev/null", O_RDWR);
  if(-1 == null)
  {
    ::perror("/dev/null");
    ::exit(EXIT_FAILURE);
  }

  if (wait_flag)
    ::signal(SIGTERM, [](int){ ::exit(EXIT_SUCCESS); });
  pid_t pid = ::fork();
  switch(pid)
  {
  case 0:
    if (wait_flag)
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
    if ( wait_flag )
      ::wait(&status);
    ::exit(status);
  }

  pid_t ppid = ::getppid();
  return [ppid, wait_flag]() noexcept
  {
    if ( wait_flag )
      ::kill( ppid, SIGTERM);
  };
}

namespace
{
  std::atomic_bool autoup_flag(false);
  std::atomic<pid_t> child_pid(0);

  static void kill_child_and_maybe_exit(int sig)
  {
    kill(child_pid, sig);
    if ( sig!=SIGHUP )
    {
      autoup_flag = false;
      ::waitpid(child_pid, nullptr, 0);
    }
  }
}

bool autoup(time_t timeout, bool success_autoup,
            std::function<bool(pid_t pid, int, int, time_t)> before,
            std::function<void(int, int, time_t)> after )
{
  if ( autoup_flag == true )
    return false;

  autoup_flag = true;
  child_pid = 0;

  int status = 0;
  time_t worktime = 0;
  for ( int i=0; true ; ++i )
  {
    time_t t = std::time(nullptr);
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

      if ( i!=0 && after!=nullptr )
      {
        int code = 0;
        if ( WIFEXITED(status) )
          code = WEXITSTATUS(status);

        after(i, code, worktime);
      }
      return false;
    }

    status = 0;

    if ( child_pid == 0)
    {
      signal(SIGHUP,  kill_child_and_maybe_exit);
      signal(SIGINT,  kill_child_and_maybe_exit);
      signal(SIGTERM, kill_child_and_maybe_exit);
    }
    child_pid = pid;

    ::waitpid(pid, &status, 0);

    if ( autoup_flag == false )
      break;

    worktime = std::time(nullptr) - t;
    bool restart = ( status!=0 || success_autoup) && ( worktime >= timeout );
    if ( restart && before!=nullptr )
    {
      int code = 0;
      if ( WIFEXITED(status) )
        code = WEXITSTATUS(status);
      restart = before(pid, i+1, code, worktime);
    }

    kill(pid, SIGKILL);
    if ( !restart )
      break;
  }
  return true;
}

int dumpable(bool enable)
{
#ifdef HAVE_SYS_PRCTL_H
    if ( enable )
    {
      rlimit core = { RLIM_INFINITY, RLIM_INFINITY };
      return ::prctl(PR_SET_DUMPABLE, 1) || ::setrlimit(RLIMIT_CORE, &core) ? -1 : 0;
    }
    else
    {
      return ::prctl(PR_SET_DUMPABLE, 0);
    }
#else
  return -2;
#endif
}

bool change_user(const std::string& username, std::string* err)
{
  passwd *pwd = ::getpwnam(username.c_str() ); /* don't free, see getpwnam() for details */
  if( pwd == nullptr )
  {
    if (err!=nullptr)
      *err = strerror(errno);
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

bool change_working_directory(const std::string& working_directory, std::string* err)
{
  if ( 0 != ::chdir( working_directory.c_str() ) )
  {
    if (err!=nullptr)
      *err = strerror(errno);
    return false;
  }
  return true;
}


}
