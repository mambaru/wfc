#include "get_procstat.hpp"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

namespace wfc{ namespace detail{

pid_t get_thread_pid()
{
  return syscall(SYS_gettid);
}

int get_pids_threads(std::vector<pid_t>& pids)
{
  std::stringstream ss;
  ss << "/proc/" << ::getpid() << "/task";
  std::string dirname = ss.str();
  boost::system::error_code ec;
  boost::filesystem::directory_iterator beg( dirname, ec), end;
  if (ec)
    return -1;
  
  std::for_each(beg, end, [&pids](const boost::filesystem::directory_entry& de)
  {
    boost::filesystem::path p(de);
    if ( boost::filesystem::is_directory(p) )
    {
      pid_t pid = ::atoi( p.filename().c_str() );
      if ( pid > 0 )
        pids.push_back(pid);
    }
  });
  return 0;
}
}}
