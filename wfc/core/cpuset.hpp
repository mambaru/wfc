//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <mutex>
#include <map>
#include <set>

namespace wfc{
  
class cpuset
{
  typedef std::mutex mutex_type;
  typedef std::set<pid_t> pid_set;
  typedef std::set<int> cpu_set;
  struct cpu_pids
  {
    cpu_set cpu;
    pid_set pids;
  };
  
public:
  void set_cpu(std::string group, const cpu_set& cpu);
  void set_current_thread(std::string group);
  void del_current_thread();
  pid_set get_pids() const;
  cpu_set get_cpu(pid_t pid) const;
  std::string get_name(pid_t pid) const;
  bool clean_dirty();
  void clear();
private:
  void del_thread_(pid_t pid);
  static pid_t get_thread_pid_();
  mutable mutex_type _mutex;
  
private:
  std::map<std::string, cpu_pids > _cpu_map;
  bool _dirty = false;
};

}
