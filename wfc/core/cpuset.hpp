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
#include <vector>

namespace wfc{
  
class cpuset
{
  typedef std::mutex mutex_type;
  typedef std::set<pid_t> pid_set;
  typedef std::set<int> cpu_set;
  typedef std::chrono::time_point<std::chrono::system_clock> time_point_t;

  struct cpu_pids
  {
    cpu_set cpu;
    pid_set pids;
  };
  
public:

  struct active_thread
  {
    pid_t pid = 0;
    std::string name; // = "группа.<<pid>>"
    time_point_t last_active;
  };

  /** @brief Отслеживание активности потоков выполнения
   *  @param pid текущего потока. Если 0 то pid определяется через системный вызов
   *  @return pid текущего потока. Используется при последующих вызовах, чтобы избежать повторных системных вызовов
   *  Поток выполнения (thrad) переодически вызывает этот метод и для него обновляеться отметка времени. Если отметка
   *  времени долго не обновляеться, то можно предположить, что поток подвис
   */
  pid_t thread_active(pid_t pid);

  /** @brief список потоков с отметками времени
   */
  void get_active(std::vector<active_thread>* active_list);

  void set_cpu(std::string group, const cpu_set& cpu);
  void set_current_thread(std::string group);
  void del_current_thread();
  static pid_t get_thread_pid();
  pid_set get_pids() const;
  cpu_set get_cpu(pid_t pid) const;
  std::string get_name(pid_t pid) const;
  bool clean_dirty();
  void clear();
private:
  void del_thread_(pid_t pid);
  mutable mutex_type _mutex;
  
private:
  std::map<std::string, cpu_pids > _cpu_map;
  bool _dirty = false;
  std::map<pid_t, active_thread> _active_map;
};

}
