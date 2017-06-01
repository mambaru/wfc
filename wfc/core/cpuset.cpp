//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2017
//
// Copyright: See COPYING file that comes with this distribution
//


#include "cpuset.hpp"
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

bool cpuset::update_thread_list()
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

  return changed;
}

void cpuset::erase_(pid_t pid)
{
  _all_pids.erase(pid);
  _reg_pids.erase(pid);
  _unreg_pids.erase(pid);
  _cpu_by_pid.erase(pid);
  for ( auto& p : _named_pid_set )
    p.second.erase(pid);
}


void cpuset::set_reg_cpu(const std::set<int>& cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);
  _reg_cpu = cpu;
  this->update_cpu_sets_();
}

void cpuset::reg_thread(std::string group)
{
  pid_t pid = detail::get_thread_pid();
  
  std::lock_guard<mutex_type> lk(_mutex);
  this->erase_(pid);
  _all_pids.insert(pid);
  _reg_pids.insert(pid);
  _named_pid_set[group].insert(pid);
  this->update_cpu_sets_();
}

void cpuset::unreg_thread()
{
  pid_t pid = detail::get_thread_pid();
  
  std::lock_guard<mutex_type> lk(_mutex);
  this->erase_(pid);
}

void cpuset::set_unreg_cpu(const std::set<int>& cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);
  _unreg_cpu = cpu;
  this->update_cpu_sets_();
}

void cpuset::set_reg_cpu(std::string group, const cpu_set& cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( cpu.empty() )
    _named_cpu.erase(group);
  else
    _named_cpu[group] = cpu;
  this->update_cpu_sets_();
}

std::vector<pid_t> cpuset::get_unreg_pids()
{
  return std::vector<pid_t>( _unreg_pids.begin(), _unreg_pids.end() );
}

void cpuset::update_cpu_sets_()
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


void cpuset::setaffinity_(pid_t pid, const cpu_set& cpu)
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
}

}

