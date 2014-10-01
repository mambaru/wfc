#define WFC_DISABLE_GLOBAL_LOG
//#define WFC_DISABLE_CLOG

#include <unistd.h>
#include <algorithm>
#include <wfc/gateway/provider/provider.hpp>
#include <wfc/memory.hpp>
#include <wfc/thread/rwlock.hpp>

const int CALL_COUNT = 1000;
const int THREAD_COUNT = 3;
const int METHOD_COUNT = 3;

namespace request{
  struct test{
    int count = -1;
    test(){}
    test(int v):count(v){}
  };
}

namespace response{
  struct test{};
}


struct test
{
  typedef std::function<void(std::unique_ptr<response::test>)> callback1;
  typedef std::function<void(std::unique_ptr<response::test>, int, std::string)> callback2;
  typedef std::function<void()> delayed_func;
  typedef std::queue<delayed_func> callback_queue;
  typedef std::recursive_mutex mutex_type;
  
  bool async_mode = false;
  callback_queue queue;

  std::atomic<int> count1;
  std::atomic<int> count2;
  std::atomic<int> count3;
  
  mutex_type mutex;

  void method1(
    std::unique_ptr<request::test> , 
    callback1 callback
  )
  {
   /* std::cout << "test::method income p->count=" << p->count << std::endl;
    if ( p->count != count1 )
    {
      std::cout<< "method1 fail count1=" << count1 << " p->count=" << p->count << std::endl;
      abort();
    }*/
    
    //std::cout << "test::method income" << std::endl;
    auto f = [this,callback]()
    {
      ++this->count1;
      //std::cout << "test::method1" << std::endl;
      
      callback(std::make_unique<response::test>());
    };
    
    if (async_mode) 
      push_(f);
    else
      f();
  }
  
  void push_(std::function<void()> f)
  {
    //std::cout << "test::push_ income" << std::endl;
    std::lock_guard<mutex_type> lk(mutex);
    //std::cout << "test::push_ {" << std::endl;
    queue.push(f);
    //std::cout << "}test::push_" << std::endl;
  }
  
  void method2(
    std::unique_ptr<request::test>, 
    callback2 callback
  )
  {
    auto f = [this, callback]()
    {
      //std::cout << "test::method2" << std::endl;
      ++this->count2;
      callback(std::make_unique<response::test>(), 42, "42");
    };
    
    if (async_mode) 
      push_(f);
    else
      f();
  }

  void method3(
    std::unique_ptr<request::test>, 
    callback2 callback,
    int p1,
    std::string p2
  )
  {
    auto f = [this, callback, p1, p2]()
    {
      //std::cout << "test::method3" << std::endl;
      ++this->count3;
      callback(std::make_unique<response::test>(), p1, p2);
    };
    
    if (async_mode) 
      push_(f);
    else
      f();
  }
  
  bool callback_one()
  {
    delayed_func f = nullptr;
    {
      std::lock_guard<mutex_type> lk(mutex);
      if (queue.empty())
        return false;
      // std::cout << "test::callback_one(){" << std::endl;
      f = queue.front();
      queue.pop();
      
      if ( queue.size() != 0 )
      {
        //std::cout << " queue.size() = " << queue.size() << std::endl;
        // для sequenced провайдера
        //abort();
      }
    }
    f();
    // std::cout << "}test::callback_one()" << std::endl;
    return true;
  }

  void callback_all()
  {
    // std::lock_guard<mutex_type> lk(mutex);
    //std::cout << "test::callback_one(){" << std::endl;
    while (callback_one());
    //std::cout << "}test::callback_one()" << std::endl;
  }
  
  bool check(int c1, int c2, int c3)
  {
    
    if (METHOD_COUNT > 0 && c1!=count1) return false;
    if (METHOD_COUNT > 1 && c2!=count2) return false;
    if (METHOD_COUNT > 2 && c3!=count3) return false;
    return true;
  }

};

typedef ::wfc::gateway::provider<test> provider_type;
typedef ::wfc::gateway::provider_config provider_config;


/// ////////////////////////////////
/// simple
/// ////////////////////////////////

void simple_test1()
{
  provider_config conf;
  conf.mode = wfc::gateway::provider_mode::sequenced;
  conf.timeout_ms = 500;
  provider_type provider(conf);
  std::atomic<size_t> callback_count(0);
  std::atomic<size_t> callback1_count(0);
  std::atomic<size_t> callback2_count(0);
  std::atomic<size_t> callback3_count(0);
    auto t = std::make_shared<test>();


  auto testing = [&]()->bool {
    std::cout << callback_count << "*" << METHOD_COUNT*THREAD_COUNT*CALL_COUNT 
              << " dropped: " << provider.get_sequenced()->dropped() 
              << " lost: "<< provider.get_sequenced()->lost_callback() 
              << " queueu:"<< provider.get_sequenced()->queue_size()
              << " t->count1:" << t->count1 
              <<  std::endl;
    return callback_count < METHOD_COUNT*THREAD_COUNT*CALL_COUNT + provider.get_sequenced()->lost_callback() + provider.get_sequenced()->dropped();
  };
  test::callback1 c1 = [&](std::unique_ptr<response::test>)
  {
    ++callback_count;
    ++callback1_count;
    //std::cout << "test::callback1 - " << callback1_count << std::endl;
  };

  test::callback2 c2 = [&](std::unique_ptr<response::test>, int p, std::string s)
  {
    if (p!=42)
      abort();
    
    if (s!="42")
      abort();
    
    ++callback_count;
    ++callback2_count;
    //std::cout << "test::callback2 - " << callback2_count << std::endl;
  };

  test::callback2 c3 = [&](std::unique_ptr<response::test>, int p, std::string s)
  {
    if (p!=24)
      abort();
    
    if (s!="24")
      abort();
    
    ++callback_count;
    ++callback3_count;
    //std::cout << "test::callback2 - " << callback3_count << std::endl;
  };
  t->async_mode = true;
  provider.startup(1, t);
  
  typedef ::wfc::rwlock<std::mutex> mutex_type;
  mutex_type mutex;
  
  auto post_one = [&]()
  {
    //std::cout << "-------------------------------------" << std::endl;
    if ( METHOD_COUNT > 0 )
      provider.post(&test::method1, std::make_unique<request::test>(0), c1);  
    if ( METHOD_COUNT > 1 )
      provider.post(&test::method2, std::make_unique<request::test>(0), c2);
    if ( METHOD_COUNT > 2 )
      provider.post(&test::method3, std::make_unique<request::test>(0), c3, 24, std::string("24"));
  };
  
  auto thread_func = [&]()
  {
    ::wfc::read_lock<mutex_type> lk(mutex);
    for (int i=0; i < CALL_COUNT; ++i)
      post_one();
  };
  
  auto callback_func = [&]{
    ::wfc::read_lock<mutex_type> lk(mutex);
    while ( /*callback_count < METHOD_COUNT*THREAD_COUNT*CALL_COUNT*/ testing())
    {
      usleep(2000);
      //std::cout << "callback_count: " << callback_count << std::endl;
      t->callback_all();
    }
    usleep(100);
    t->callback_all();
  };
  
  mutex.lock();
  
  std::list<std::thread> thread_list;
  for (int i =0; i < THREAD_COUNT; ++i )
  {
    thread_list.push_back( std::thread(thread_func) );
    thread_list.push_back( std::thread(callback_func) );
  }
  
  thread_list.push_front( std::thread([&]()
  {
    ::wfc::read_lock<mutex_type> lk(mutex);
    size_t client_id = 1;
    while ( /*callback_count != METHOD_COUNT*THREAD_COUNT*CALL_COUNT*/testing())
    {
      
      usleep(30);
      provider.shutdown(client_id);
      usleep(30);
      client_id = callback_count + 1;
      provider.startup(client_id, t);
    };
  }));
  
  usleep(10000);
  mutex.unlock();
  std::for_each(thread_list.begin(), thread_list.end(), [](std::thread& th) { th.join();});
  
  /*
  std::thread th1(thread_func);
  std::thread th2(thread_func);
  std::thread th3(thread_func);
  std::thread th4(callback_func);
  th1.join();
  th2.join();
  th3.join();
  th4.join();
  */
  
  std::cout << "count1: " << t->count1 << std::endl;
  std::cout << "count2: " << t->count2 << std::endl;
  std::cout << "count3: " << t->count3 << std::endl;

  std::cout << "callback count1: " << callback1_count << std::endl;
  std::cout << "callback count2: " << callback2_count << std::endl;
  std::cout << "callback count3: " << callback3_count << std::endl;
  
  std::cout << "callback total: " << callback_count << std::endl;
  std::cout << "lost_call: " << provider.get_sequenced()->lost_call() << std::endl;
  std::cout << "lost_callback: " << provider.get_sequenced()->lost_callback() << std::endl;

  if ( !t->check(THREAD_COUNT*CALL_COUNT, THREAD_COUNT*CALL_COUNT, THREAD_COUNT*CALL_COUNT))
  {
    abort();
  }
  
  
  if (METHOD_COUNT > 0 &&  callback1_count!=THREAD_COUNT*CALL_COUNT)
    abort();
  if (METHOD_COUNT > 1 &&  callback2_count!=THREAD_COUNT*CALL_COUNT)
    abort();
  if (METHOD_COUNT > 2 &&  callback3_count!=THREAD_COUNT*CALL_COUNT)
    abort();
  if (callback_count!=THREAD_COUNT*CALL_COUNT*METHOD_COUNT)
    abort();
  
  t.reset();
  
}

void simple_test()
{
  simple_test1();
}

/// ////////////////////////////////
/// squence
/// ////////////////////////////////

int main()
{
  simple_test();
  return 0;
}
