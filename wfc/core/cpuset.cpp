//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#include "cpuset.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include <iostream>

namespace wfc{

pid_t cpuset::thread_active(pid_t pid)
{
  std::lock_guard<mutex_type> lk(_mutex);

  if ( pid == 0 )
    pid = cpuset::get_thread_pid();

  auto& ap = _active_map[pid];
  ap.pid = pid;
  ap.last_active = std::chrono::system_clock::now();
  return pid;
}

void cpuset::get_active(std::vector<active_thread>* active_list)
{
  std::lock_guard<mutex_type> lk(_mutex);
  if (active_list!=nullptr)
  {
    active_list->reserve( _active_map.size() );
    for (const auto& item : _active_map)
    {
      if ( item.second.pid != 0 )
      {
        active_list->push_back(item.second);
        active_list->back().name = item.second.name + "." + std::to_string(item.first);
      }
    }
  }
}


bool cpuset::clean_dirty()
{
  std::lock_guard<mutex_type> lk(_mutex);
  bool pred = _dirty;
  _dirty = false;
  return pred;
}

void cpuset::set_cpu(std::string group, const cpu_set& cpu)
{
  std::lock_guard<mutex_type> lk(_mutex);
  _dirty = true;
  _cpu_map[group].cpu = cpu;
}

void cpuset::set_current_thread(std::string group)
{
  std::lock_guard<mutex_type> lk(_mutex);
  _dirty = true;
  pid_t pid = cpuset::get_thread_pid();
  this->del_thread_(pid);
  _cpu_map[group].pids.insert(pid);
  _active_map[pid].name = group;
  _active_map[pid].last_active = std::chrono::system_clock::now();
}

void cpuset::del_current_thread()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _dirty = true;
  pid_t pid = cpuset::get_thread_pid();
  _active_map.erase(pid);
  this->del_thread_(pid);
}

cpuset::pid_set cpuset::get_pids() const
{
  std::lock_guard<mutex_type> lk(_mutex);
  cpuset::pid_set pids;
  for ( const auto& p : _cpu_map)
    pids.insert( p.second.pids.begin(), p.second.pids.end() );
  return pids;
}

cpuset::cpu_set cpuset::get_cpu(pid_t pid) const
{
  std::lock_guard<mutex_type> lk(_mutex);
  for ( const auto& p : _cpu_map)
    if ( p.second.pids.count(pid)!=0 )
      return p.second.cpu;
  return cpu_set();
}

std::string cpuset::get_name(pid_t pid) const
{
  std::lock_guard<mutex_type> lk(_mutex);
  for ( const auto& p : _cpu_map)
    if ( p.second.pids.count(pid)!=0 )
      return p.first;
  return "";
}

void cpuset::del_thread_(pid_t pid)
{
  for (auto& s : _cpu_map)
    s.second.pids.erase(pid);
}

pid_t cpuset::get_thread_pid()
{
  return static_cast<pid_t>(syscall(SYS_gettid));
}

void cpuset::clear()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _cpu_map.clear();
}

}
