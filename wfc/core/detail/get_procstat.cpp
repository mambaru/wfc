#include "get_procstat.hpp"
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

struct s;

namespace wfc{ namespace detail{

namespace {
  
void sscanf_stat(const char *t, procstat* pinfo)
{
  sscanf (t, "%c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
          /*       1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33*/
          &(pinfo->state),
          &(pinfo->ppid),
          &(pinfo->pgrp),
          &(pinfo->session),
          &(pinfo->tty),
          &(pinfo->tpgid),
          &(pinfo->flags),
          &(pinfo->minflt),
          &(pinfo->cminflt),
          &(pinfo->majflt),
          &(pinfo->cmajflt),
          &(pinfo->utime),
          &(pinfo->stime),
          &(pinfo->cutime),
          &(pinfo->cstime),
          &(pinfo->counter),
          &(pinfo->priority),
          &(pinfo->timeout),
          &(pinfo->itrealvalue),
          &(pinfo->starttime),
          &(pinfo->vsize),
          &(pinfo->rss),
          &(pinfo->rlim),
          &(pinfo->startcode),
          &(pinfo->endcode),
          &(pinfo->startstack),
          &(pinfo->kstkesp),
          &(pinfo->kstkeip),
          &(pinfo->signal),
          &(pinfo->blocked),
          &(pinfo->sigignore),
          &(pinfo->sigcatch),
          &(pinfo->wchan));  
}

int read_stat( std::string name, procstat* pinfo)
{
  std::ifstream ifs(name);
  if ( !ifs )
    return -1;
  std::string stat;
  //stat = ifs.getline();
  std::getline(ifs, stat);
  ::sscanf ( stat.c_str(), "%d", &(pinfo->pid) );
  std::cout << stat << std::endl;
  size_t left = stat.find_first_of('(');
  std::cout << left << std::endl;
  size_t right = stat.find_first_of(')');
  std::cout << right << std::endl;
  stat.substr( left, right - left );
  sscanf_stat( stat.c_str() + right + 2, pinfo);
  return 0;
}

std::string get_procname()
{
  std::stringstream ss;
  ss << "/proc/" << ::getpid() << "/stat";
  return ss.str();
}

} // namespace 

int get_procstat(procstat* pinfo)
{
  return read_stat(get_procname(), pinfo);
}

}}
