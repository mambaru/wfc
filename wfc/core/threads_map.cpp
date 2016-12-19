//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#include "threads_map.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string.h>
#include <wfc/logger.hpp>
#include "detail/get_procstat.hpp"
#include "detail/procstat.hpp"

namespace wfc{

void threads_map::set_cpu(std::vector<int> cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);  
}

std::vector<int> threads_map::get_cpu()
{
  std::lock_guard<mutex_type> lk(_mutex);
}

void threads_map::set_self()
{
  std::lock_guard<mutex_type> lk(_mutex);
  std::thread::id id = std::this_thread::get_id();
  pid_t pid = ::syscall(SYS_gettid);
  _std[id] = pid;
  _pid[pid] = id;
  
  cpu_set_t  mask;
  CPU_ZERO(&mask);
  
  for (int cpu : _cpu )
    CPU_SET(cpu, &mask);

  int result = ::sched_setaffinity( pid, sizeof(mask), &mask);
  if ( result == -1 )
  {
    CONFIG_LOG_ERROR("sched_setaffinity: " << strerror(errno) )
  }
}

std::vector<pid_t> threads_map::get_pids() const
{
  std::lock_guard<mutex_type> lk(_mutex);  
}

std::vector<std::thread::id> threads_map::get_ids() const
{
  std::lock_guard<mutex_type> lk(_mutex);
  
}

namespace {

  threads_map::thread_stat init_stat( detail::procstat p )
  {
    threads_map::thread_stat ts;
    ts.pid = p.pid;
    ts.rss = p.rss;
    ts.stime = p.stime;
    ts.utime = p.utime;
    ts.vsize = p.vsize;
    return ts;
  }  
}

threads_map::thread_stat threads_map::get_proc_stat()
{
  detail::procstat p;
  get_procstat(&p);
  thread_stat ts = init_stat(p);
  pid_t pid = ::getpid();
  std::lock_guard<mutex_type> lk(_mutex);
  auto itr = _stat.find(pid);
  if ( itr == _stat.end() )
    itr = _stat.insert( std::make_pair(pid, ts) ).first;
  thread_stat res = ts;
  res.stime -= itr->second.stime;
  res.utime -= itr->second.utime;
  itr->second = ts;
  return res;
}

}

