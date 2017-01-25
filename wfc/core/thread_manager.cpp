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
//#include "detail/procstat.hpp"


#include <sys/resource.h>
#include <sched.h>

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

/*
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
*/

bool thread_manager::update_thread_list()
{
  bool changed = false;
  std::vector<pid_t> pids;
  detail::get_pids_threads( pids );

  std::lock_guard<mutex_type> lk(_mutex);
  
  auto trash = this->_all_pids;
  for (auto pid : pids )
  {
    trash.erase(pid);
    if ( _all_pids.insert(pid).second )
    {
      // Если незарегистрированный поток
      auto itr = _reg_pids.find( pid );
      if ( itr == _reg_pids.end() && _unreg_pids.insert(pid).second )
       changed =true;
    }
  }

  if ( !trash.empty() )
    changed = true;
    
  // Удаляем старые потоки
  for (auto pid : trash )
    this->erase_(pid);
  
  this->update_cpu_sets_();
  /*
  for (auto pid : update_cpu )
  {
    const  auto &cpu = _cpu_by_pid[pid];
    cpu_set_t  mask;
    CPU_ZERO(&mask);
    for (int id : cpu )
      CPU_SET(id, &mask);
    ::sched_setaffinity( ::getpid(), sizeof(mask), &mask);
  }
  */
  
  return changed;
}

void thread_manager::erase_(pid_t pid)
{
  _all_pids.erase(pid);
  _reg_pids.erase(pid);
  _unreg_pids.erase(pid);
  _cpu_by_pid.erase(pid);
  for ( auto& p : _named_pid_set )
    p.second.erase(pid);
}


void thread_manager::set_reg_cpu(const std::set<int>& cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);
  _reg_cpu = cpu;
  this->update_cpu_sets_();
}

void thread_manager::reg_thread(std::string group)
{
  pid_t pid = detail::get_thread_pid();
  
  std::lock_guard<mutex_type> lk(_mutex);
  this->erase_(pid);
  _all_pids.insert(pid);
  _reg_pids.insert(pid);
  _named_pid_set[group].insert(pid);
  /*auto itr = _named_pid_set.find(group);
  if ( itr != _named_pid_set.end() )
  {
    _cpu_by_pid[pid] = itr->second;
  }
  else
  {
    _cpu_by_pid[pid] = _reg_cpu;
  }
  */
  this->update_cpu_sets_();
}

void thread_manager::unreg_thread()
{
  pid_t pid = detail::get_thread_pid();
  
  std::lock_guard<mutex_type> lk(_mutex);
  this->erase_(pid);
}

void thread_manager::set_unreg_cpu(const std::set<int>& cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);
  _unreg_cpu = cpu;
  this->update_cpu_sets_();
}

void thread_manager::set_reg_cpu(std::string group, const cpu_set& cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( cpu.empty() )
    _named_cpu.erase(group);
  else
    _named_cpu[group] = cpu;
  this->update_cpu_sets_();
}

std::vector<pid_t> thread_manager::get_unreg_pids()
{
  return std::vector<pid_t>( _unreg_pids.begin(), _unreg_pids.end() );
}

void thread_manager::update_cpu_sets_()
{
  for ( pid_t p : _unreg_pids )
    _cpu_by_pid[p] = _unreg_cpu;
  for ( pid_t p : _reg_pids )
    _cpu_by_pid[p] = _reg_cpu;

  // Обновление именованых настроек
  for ( const auto& n : _named_cpu )
  {
    auto itr = _named_pid_set.find(n.first);
    if ( n.second.empty() )
      continue;
    if ( itr != _named_pid_set.end() )
    {
      for ( auto p : itr->second )
        _cpu_by_pid[p] = n.second;
    }
  }
  
  for ( auto& p : _cpu_by_pid)
    this->setaffinity_(p.first, p.second);
  this->setaffinity_( ::getpid(), _reg_cpu);
}


void thread_manager::setaffinity_(pid_t pid, const cpu_set& cpu)
{
  std::stringstream ss;
  ss << "CPUs for pid=" << pid << " [";
  cpu_set_t  mask;
  CPU_ZERO(&mask);
  for (int id : cpu )
  {
    ss << id << ",";
    CPU_SET(id, &mask);
  }
  ss << "]";
  ::sched_setaffinity( pid, sizeof(mask), &mask);
  // COMMON_LOG_MESSAGE(ss.str())
}

/*
thread_manager::statistics thread_manager::process_statistics()
{
  detail::procstat p;
  get_procstat(&p);
  return init_stat(p);
}
*/

}

