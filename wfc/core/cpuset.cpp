//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#include "cpuset.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>

namespace wfc{

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
  pid_t pid = this->get_thread_pid_();
  this->del_thread_(pid);
  _cpu_map[group].pids.insert(pid);
}

void cpuset::del_current_thread()
{
  std::lock_guard<mutex_type> lk(_mutex);
  _dirty = true;
  pid_t pid = this->get_thread_pid_();
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

pid_t cpuset::get_thread_pid_()
{
  return static_cast<pid_t>(syscall(SYS_gettid));
}

}
