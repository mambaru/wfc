//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#include "thread_manager.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string.h>
#include <wfc/logger.hpp>
#include "detail/get_procstat.hpp"
#include "detail/procstat.hpp"

namespace wfc{

  /*
void thread_manager::set_cpu(std::vector<int> )
{
  std::lock_guard<mutex_type> lk(_mutex);  
}

std::vector<int> thread_manager::get_cpu()
{
  std::lock_guard<mutex_type> lk(_mutex);
  return std::vector<int>();
}

void thread_manager::set_self()
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

std::vector<pid_t> thread_manager::get_pids() const
{
  std::lock_guard<mutex_type> lk(_mutex);  
  return std::vector<pid_t>();
}

std::vector<std::thread::id> thread_manager::get_ids() const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return std::vector<std::thread::id>();
}
  */
  
namespace {

  thread_manager::statistics init_stat( detail::procstat p )
  {
    thread_manager::statistics ts;
    ts.pid = p.pid;
    ts.rss = p.rss;
    ts.stime = p.stime;
    ts.utime = p.utime;
    ts.vsize = p.vsize;
    return ts;
  }  
}

bool thread_manager::update_thread_list()
{
  bool changed = false;
  std::vector<pid_t> pids;
  detail::get_pids_threads( pids );
  std::lock_guard<mutex_type> lk(_mutex);
  auto trash = this->_all_pids;
  // потоки для обновления распределения по cpu
  pid_set update_cpu;
  for (auto pid : pids )
  {
    if ( _all_pids.insert(pid).second )
      changed = true;
    
    // Если незарегистрированный поток
    auto itr = _pid_by_id.find( pid );
    if ( itr == _pid_by_id.end() )
    {
      _unreg_pids.insert(pid);
      _cpu_by_pid[pid] = _unreg_cpu;
      update_cpu.insert(pid);
    }
    trash.erase(pid);
  }

  if ( !trash.empty() )
    changed = true;
    
  // Удаляем старые потоки
  for (auto pid : trash )
  {
    pid = 0;
  }
  // TODO: обновить установки cpu
  return changed;
}

thread_manager::statistics thread_manager::process_statistics()
{
  detail::procstat p;
  get_procstat(&p);
  return init_stat(p);
}

}

