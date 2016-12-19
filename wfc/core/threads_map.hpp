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


namespace wfc{

  
class threads_map
{
  typedef std::mutex mutex_type;
public:

  struct thread_stat
  {
    size_t utime = 0;                    /** user mode jiffies **/
    size_t stime = 0;                    /** kernel mode jiffies **/
    size_t vsize = 0;                    /** Virtual memory size **/
    size_t rss  = 0;                      /** Resident Set Size **/
    int pid = 0;
  };

  void set_cpu(std::vector<int> cpu);
  std::vector<int> get_cpu();
  void set_self();
  std::vector<pid_t> get_pids() const;
  std::vector<std::thread::id> get_ids() const;
  
  thread_stat get_proc_stat();
public:
  mutable mutex_type _mutex;
  std::vector<int> _cpu;
  std::map<std::thread::id, pid_t> _std;
  std::map<pid_t, std::thread::id> _pid;
  std::map<pid_t, thread_stat>     _stat;
  
};

}
