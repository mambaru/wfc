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

  
class thread_manager
{
  typedef std::mutex mutex_type;
  typedef std::set<pid_t> pid_set;
  typedef std::set<int> cpu_set;
  
public:

  struct statistics
  {
    size_t utime = 0;                    /** user mode jiffies **/
    size_t stime = 0;                    /** kernel mode jiffies **/
    size_t vsize = 0;                    /** Virtual memory size **/
    size_t rss  = 0;                      /** Resident Set Size **/
    int pid = 0;
    
    statistics operator - (const statistics& t) const
    {
      statistics r;
      r.utime = utime - t.utime;                    /** user mode jiffies **/
      r.stime = stime - t.stime;                    /** kernel mode jiffies **/
      r.vsize = vsize - t.vsize;                    /** Virtual memory size **/
      r.rss   = rss - t.rss;                      /** Resident Set Size **/
      r.pid   = t.pid;
      return r;
    }
  };

  /*
  void set_cpu(std::vector<int> cpu);
  std::vector<int> get_cpu();
  void set_self();
  std::vector<pid_t> get_pids() const;
  std::vector<std::thread::id> get_ids() const;
  */
  void set_reg_cpu(const cpu_set& cpu);
  void set_unreg_cpu(const cpu_set&  cpu);
  bool update_thread_list(); 
  statistics process_statistics();
private:
  void update_cpu_sets_();
  void setaffinity_(pid_t pid, const cpu_set& cpu);
  void erase_(pid_t pid);
public:
  mutable mutex_type _mutex;
  
  // Зарегестрированные потоки
  /*std::map<std::thread::id, pid_t> _id_by_pid;
  std::map<pid_t, std::thread::id> _id_by_pid;
  */
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

}
