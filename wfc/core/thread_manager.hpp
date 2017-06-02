//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <set>


namespace wfc{

  /*
class thread_manager
{
  typedef std::mutex mutex_type;
  typedef std::set<pid_t> pid_set;
  typedef std::set<int> cpu_set;
public:
  void set_reg_cpu(std::string group, const cpu_set& cpu);
  void set_reg_cpu(const cpu_set& cpu);
  void set_unreg_cpu(const cpu_set&  cpu);
  void reg_thread(std::string group);
  void unreg_thread();
  bool update_thread_list(); 

  std::vector<pid_t> get_unreg_pids();

private:

  void update_cpu_sets_();
  void setaffinity_(pid_t pid, const cpu_set& cpu);
  void erase_(pid_t pid);
public:
  mutable mutex_type _mutex;
  
  // Все потоки
  pid_set _all_pids;
  // Не зарегестрированные потоки
  pid_set _unreg_pids;
  // Зарегестрированные потоки
  pid_set _reg_pids;
  // Именованые наборы потоков
  std::map<std::string, pid_set > _named_pid_set;
  std::map<std::string, cpu_set > _named_cpu;
  
  // Настройки CPU для зарегистрированных
  cpu_set _reg_cpu;
  // Настройки CPU для не зарегистрированных
  cpu_set _unreg_cpu;
  // Индивидуальные настройки CPU
  std::map<pid_t, cpu_set > _cpu_by_pid;
};
*/
}
