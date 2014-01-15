#pragma once

#include <wfc/inet/srv/work_thread.hpp>
#include <wfc/thread/rwlock.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <vector>
#include <thread>
#include <atomic>


namespace wfc{ namespace inet{
  
struct ad_worker
{
  typedef rwlock<std::mutex> mutex_type;
  
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef std::function<void(socket_type)> func_type;
  typedef std::vector< std::unique_ptr<work_thread> > work_thread_vector;
  
  /// initialize
  
  template<typename T>
  void operator()(T& , fas::tag<_stop_> )
  {
    std::cout << "ad_worker::stop" << std::endl;
    for(auto& thr : _work_threads )
    {
      thr->stop();
    }
    _work_threads.clear();
  }

    
  template<typename T>
  void operator()(T& t, fas::tag<_startup_> )
  {
    int worker_threads = t.server_context().worker_threads;
    std::lock_guard<mutex_type> lk(_mutex);
    _index = 0;
    _max_index = worker_threads;
    _work_threads.resize(worker_threads);
    for (int i=0; i < worker_threads; ++i)
    {
      _work_threads[i] = std::make_unique<work_thread>();
      _work_threads[i]->start();
    }
  }
  
  template<typename T>
  void operator()(T&, socket_type sock , func_type fun)
  {
    read_lock<mutex_type> lk(_mutex);
    
    if ( _max_index == 0 )
    {
      fun( std::move(sock) );
    }
    else
    {
      // чтобы не делать write lock
      int current = _index++;
      
      if (current>=_max_index)
        current = 0;
      
      if (_index>=_max_index)
        _index = 0;

      _work_threads[current]->post(std::move(sock),  fun);
    }
  }

private:
  mutex_type _mutex;
  work_thread_vector _work_threads;
  std::atomic<int> _index;
  std::atomic<int> _max_index;
};

}}
