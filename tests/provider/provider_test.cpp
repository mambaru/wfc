#define WFC_DISABLE_GLOBAL_LOG
#define WFC_DISABLE_CLOG

#include <unistd.h>
#include <algorithm>
#include <wfc/gateway/provider/provider.hpp>
#include <wfc/memory.hpp>
#include <wfc/thread/rwlock.hpp>

#include <queue>
///
/// API
///

struct request
{
  int src_id = -1;
  int count = -1;
};

struct response
{
  int src_id = -1;
  int dst_id = -1;
  int count = -1;
};

typedef std::unique_ptr<request> request_ptr;
typedef std::unique_ptr<response> response_ptr;

///
/// inteface
///

struct ireceiver
{
  typedef std::function<void(response_ptr)>                   callback1;
  typedef std::function<void(response_ptr, int, int)>         callback2;
  typedef std::function<void(response_ptr, int, std::string)> callback3;
  
  virtual void method1(request_ptr req, callback1 callback ) = 0;
  virtual void method2(request_ptr req, callback2 callback ) = 0;
  virtual void method3(request_ptr req, callback3 callback, int, std::string ) = 0;
};

///
/// счетчики
/// 

struct receiver_counters
{
  receiver_counters()
    : call_counter1(0)
    , call_counter2(0)
    , call_counter3(0)
    , callback_counter1(0)
    , callback_counter2(0)
    , callback_counter3(0)
  {}
  
  receiver_counters(const receiver_counters& other)
    : call_counter1(other.call_counter1.load())
    , call_counter2(other.call_counter2.load())
    , call_counter3(other.call_counter3.load())
    , callback_counter1(other.callback_counter1.load())
    , callback_counter2(other.callback_counter2.load())
    , callback_counter3(other.callback_counter3.load())
  {}

  void operator += (const receiver_counters& other)
  {
    call_counter1 += other.call_counter1.load();
    call_counter2  += other.call_counter2.load();
    call_counter3 +=  other.call_counter3.load();
    callback_counter1 += other.callback_counter1.load();
    callback_counter2 += other.callback_counter2.load();
    callback_counter3 += other.callback_counter3.load();
  }

  std::atomic<int> call_counter1;
  std::atomic<int> call_counter2;
  std::atomic<int> call_counter3;
  std::atomic<int> callback_counter1;
  std::atomic<int> callback_counter2;
  std::atomic<int> callback_counter3;
};

struct source_counters
{
  source_counters()
    : send_counter1(0)
    , send_counter2(0)
    , send_counter3(0)
    , response_counter1(0)
    , response_counter2(0)
    , response_counter3(0)
  {}
  
  source_counters(const source_counters& other)
    : send_counter1(other.send_counter1.load())
    , send_counter2(other.send_counter2.load())
    , send_counter3(other.send_counter3.load())
    , response_counter1(other.response_counter1.load())
    , response_counter2(other.response_counter2.load())
    , response_counter3(other.response_counter3.load())
  {}

  void operator += (const source_counters& other)
  {
    send_counter1 += other.send_counter1.load();
    send_counter2  += other.send_counter2.load();
    send_counter3 +=  other.send_counter3.load();
    response_counter1 += other.response_counter1.load();
    response_counter2 += other.response_counter2.load();
    response_counter3 += other.response_counter3.load();
  }

  
  std::atomic<int> send_counter1;
  std::atomic<int> send_counter2;
  std::atomic<int> send_counter3;
  std::atomic<int> response_counter1;
  std::atomic<int> response_counter2;
  std::atomic<int> response_counter3;
};

struct provider_counters
{
  // Повторные вызовы
  int recall_count = -1;
  // Сброшенные по timeout
  int drop_count = -1;
  // Потерянные callback's
  int orphan_count = -1;
  // размер очереди
  int queue_size = -1;
  // размер очереди
  int wait_count = -1;
};


///
/// Обработчик "удаленых" запросов
///
class receiver
  : public receiver_counters
  , public ireceiver
  
{
public:
  typedef std::function<void()> delayed_func;
  typedef std::queue<delayed_func> callback_queue;
  typedef std::mutex mutex_type;

  virtual ~receiver()
  {
  }
  
///
/// options
///

  /// В async_mode = true ответы отправляет не сразу, а по запросу. см. callback_one
  bool async_mode = false;
  
  void method1(std::unique_ptr<request> req, callback1 callback )
  {
    try
    {
      response resp;
      resp.src_id = req->src_id;
      resp.dst_id = 1;
      resp.count = req->count;
      ++this->call_counter1;
      
      delayed_func f = [this, resp, callback]()
      {
        ++(this->callback_counter1);
        callback( std::make_unique<response>(resp) );
      };
      
      if (async_mode) 
      {
        push_(f);
      }
      else f();
    }
    catch(...)
    {
      std::cout << "abort1 " << std::endl;
      abort();
    }
  }

  void method2(std::unique_ptr<request> req, callback2 callback )
  {
    try
    {
      response resp;
      resp.src_id = req->src_id;
      resp.dst_id = 2;
      resp.count = req->count;
      ++this->call_counter2;
      
      delayed_func f = [this, resp, callback]()
      {
        
        ++(this->callback_counter2);
        callback( std::make_unique<response>(resp), 33, 44 );
      };
      
      if (async_mode) push_(f);
      else f();
    }
    catch(...)
    {
      std::cout << "abort2 " << std::endl;
      abort();
    }
  }

  void method3(std::unique_ptr<request> req, callback3 callback, int p1, std::string p2 )
  {
    try
    {
      response resp;
      resp.src_id = req->src_id;
      resp.dst_id = 3;
      resp.count = req->count;
      ++this->call_counter3;
      
      delayed_func f = [this, resp, callback, p1, p2]()
      {
        ++(this->callback_counter3);
        callback( std::make_unique<response>(resp), p1, p2 );
      };
      
      if (async_mode) push_(f);
      else f();
    }
    catch(...)
    {
      std::cout << "abort3 " << std::endl;
      abort();
    }
  }

///
/// helper
///
  void push_(std::function<void()> f)
  {
    std::lock_guard<mutex_type> lk(this->mutex);
    queue.push(f);
  }

  bool callback_one()
  {
    std::lock_guard<mutex_type> lk(this->mutex);
    delayed_func f = nullptr;
    {
      if (queue.empty())
        return false;
      f = queue.front();
      queue.pop();
    }
    f();
    // std::cout << "}test::callback_one()" << std::endl;
    return true;
  }
  
  bool ready_fin()
  {
    std::lock_guard<mutex_type> lk(this->mutex);
    return queue.empty();
  }
  
  receiver_counters get_counters() const
  {
    return static_cast<receiver_counters>(*this);
  }

  
  mutex_type mutex;
  callback_queue queue;

};

/// 
/// отправитель
/// 

typedef ::wfc::gateway::provider_config provider_config;
typedef ::wfc::gateway::provider_mode provider_mode;
typedef ::wfc::gateway::provider<ireceiver>  provider_type;
typedef std::shared_ptr<provider_type>  provider_ptr;


class source
  : public source_counters
{
public:
  provider_ptr provider;
  
  void send_method1()
  {
    request req;
    req.count = ++send_counter1;
    req.src_id = 1;
    ireceiver::callback1 callback = [this, req](response_ptr resp)
    {
      ++(this->response_counter1);
      this->check_result( req, *resp);
    };
    provider->post(&ireceiver::method1, std::make_unique<request>(req), callback);
  }

  void send_method2()
  {
    request req;
    req.count = ++send_counter2;
    req.src_id = 2;
    ireceiver::callback2 callback = [this, req](response_ptr resp, int p1, int p2)
    {
      ++(this->response_counter2);
      this->check_result( req, *resp);
      if ( p1 != 33 )
        abort();
      if ( p2 != 44 )
        abort();
    };
    provider->post(&ireceiver::method2, std::make_unique<request>(req), callback);
  }

  void send_method3()
  {
    request req;
    req.count = ++send_counter3;
    req.src_id = 3;
    ireceiver::callback3 callback = [this, req](response_ptr resp, int p1, std::string p2)
    {
      ++(this->response_counter3);
      this->check_result( req, *resp);
      if ( p1 != 42 )
        abort();
      if ( p2 != "42" )
        abort();
    };
    provider->post(&ireceiver::method3, std::make_unique<request>(req), callback, 42, std::string("42"));
  }

  void send_all(int mpr)
  {
    //std::cout << "send_all-1" << std::endl;
    if ( mpr > 0 )
      this->send_method1();
    //std::cout << "send_all-2" << std::endl;
    if ( mpr > 1 )
      this->send_method2();
    //std::cout << "send_all-3" << std::endl;
    if ( mpr > 2 )
      this->send_method3();
    //std::cout << "send_all-ok" << std::endl;
  }
///
///
///
  
  void check_result(const request& req, const response& resp)
  {
    if ( req.src_id != resp.src_id)
      abort();
    if ( req.src_id != resp.dst_id)
      abort();
    if ( req.count != resp.count)
      abort();
  }
  
  source_counters get_counters() const
  {
    return static_cast<source_counters>(*this);
  }
  
};

///
///
///
struct toster_config
{
  // bool dst_async_mode = false;
  // Кол-во источников
  int  dst_cout = 1;
  // Кол-во получателе
  int  src_cout = 1;
  // method_per_request=1 только method1, method_per_request=3 все три (method1, method2, method3)  
  int method_per_request = 3;
  // Число запросов из потоков
  int request_per_thread = 1000;
  // Число потоков делающих запросы
  int requester_threads_per_dst  = 3;
  // Таймауты между запросами
  int request_timeout_ms = 0;
  // Число потоков активирующие callback на receiver в асинхронном режиме
  int callback_threads  = 3;
  // Таймауты между перед отправкой ответа в async_mode
  int callback_timeout_ms = 1000;
  // таймаут разрыв (0-не используеться). На каждый источнико свой поток
  int shutdown_timeout_ms = 100000;
  // сколько быть в shutdown
  int shutdown_time_ms = 500; 
  // Конфигурация провайдера;
  provider_config provider;
};


///
/// тостер
///
class toster
{
public:
  
  typedef std::shared_ptr<receiver> receiver_ptr;
  typedef std::shared_ptr<source> source_ptr;
  typedef ::wfc::rwlock<std::mutex> barier_type;
  toster(const toster_config& conf)
    : conf(conf)
    , ready_time(0)
    , send_ready(0)
  {
    barier.lock();
    // Провайдер
    _provider = std::make_shared<provider_type>(io_service, conf.provider);
    
    // Отправитель
    for (int i = 0; i < conf.src_cout; ++i)
    {
      auto src = std::make_shared<source>();
      src->provider = _provider;
      _sources.push_back(src);
      for ( int j=0; j < conf.requester_threads_per_dst; ++j)
      {
        ++this->send_ready;
        _threads.push_back( std::thread([this, src]()
        {
          ::wfc::read_lock<barier_type> lk(this->barier);
          //std::cout << "send..." << std::endl;
          for (int i=0; i < this->conf.request_per_thread; ++i)
          {
            //std::cout << "send... " << i << std::endl;
            src->send_all(this->conf.method_per_request);
            //std::cout << "send... " << i << " ok"<< std::endl;
            if ( this->conf.request_timeout_ms > 0 )
            {
              usleep(this->conf.request_timeout_ms);
            }
          }
          --this->send_ready;
          //std::cout << "send... ready" << std::endl;
        }));
      }
    }
    // Получатель
    for (int i = 0; i < conf.dst_cout; ++i)
    {
      int client_id = i + 1;
      receiver *pdst = new receiver;
      auto dst = std::shared_ptr<receiver>(pdst);
      //auto dst = std::make_shared<receiver>();
      dst->async_mode = conf.callback_timeout_ms != 0;
      _receivers.push_back(dst);
      _provider->startup(client_id, dst);
      _threads.push_back(std::thread([this, dst]()
      {
        ::wfc::read_lock<barier_type> lk(this->barier);
        while ( !this->ready() )
        {
          if ( !dst->callback_one() )
          {
            if ( this->conf.callback_timeout_ms > 0 )
            {
              usleep(this->conf.callback_timeout_ms);
            }
          }
        }
        while ( !dst->ready_fin() )
        {
          dst->callback_one();
          //std::cout << "Очередь не пуста!" << std::endl;
        }
      }));

      if ( conf.shutdown_timeout_ms > 0 )
      {
        _threads.push_back(std::thread([this, dst, client_id]()
        {
          ::wfc::read_lock<barier_type> lk(this->barier);
          while ( !this->ready() )
          {
            this->_provider->shutdown(client_id);
            usleep(this->conf.shutdown_time_ms);
            this->_provider->startup(client_id, dst);
            usleep(this->conf.shutdown_timeout_ms);
          }
        }));
      }
    }
    // Шутдаунер
  }
  
  void run()
  {
    std::thread th([this]() { this->io_service.run(); });
    barier.unlock();
    for ( auto& th : _threads)
    {
      th.join();
      //std::cout << "th.join()" << std::endl;
    }
      /*while (1)
    {
      std::cout <<  _provider->get_sequenced()->queue_size() <<  std::endl;
      sleep(1);
    };*/
    this->io_service.stop();
    th.join();  
  }
  
  bool ready()
  {
    if ( _provider->get_sequenced()->queue_size() != 0 )
    {
      // TODO: сделат общмй метод queue_size
      //std::cout <<  "get_sequenced()->queue_size() " <<  _provider->get_sequenced()->queue_size() <<  std::endl;
      return false;
    }
    
    if ( this->send_ready != 0 )
      return false;
    //std::cout <<  "ready?" <<  std::endl;
    time_t now = time(0);
    if ( ready_time == 0 )
    {
      ready_time = now + 1 /*+ 2*/;
      return false;
    }
    return now > ready_time;
  }
  
  source_counters get_source_counters() const
  {
    source_counters sc;
    for (auto& s: _sources)
      sc += s->get_counters();
    return sc;
  }
  
  receiver_counters get_receiver_counters() const
  {
    receiver_counters rc;
    for (auto& s: _receivers)
      rc += s->get_counters();
    return rc;
  }
  
  provider_counters get_provider_counters() const
  {
    provider_counters pc;
    if ( conf.provider.mode == provider_mode::simple)
    {
      pc.recall_count = 0;
      pc.drop_count = _provider->get_simple()->drop_count();
      pc.orphan_count = 0;
      pc.queue_size = 0;
    }
    else if ( conf.provider.mode == provider_mode::sequenced)
    {
      pc.recall_count = _provider->get_sequenced()->recall_count();
      pc.drop_count = _provider->get_sequenced()->drop_count();
      pc.orphan_count = _provider->get_sequenced()->orphan_count();
      pc.queue_size = _provider->get_sequenced()->queue_size();
      pc.wait_count = _provider->get_sequenced()->wait_count();
      
    }
      
    return pc;
  }
  
  toster_config conf;
  ::wfc::io_service io_service;

  // Для таймаута завершения после отправки всех сообщений
  // Для некоторых тестов не можем сравнивать количетво отправленных и полученных,
  // т.к. допустимы потери
  std::atomic<time_t> ready_time;
  // 0 - все данные отправленны
  std::atomic<int> send_ready;
  // список получателей
  std::list<receiver_ptr> _receivers;
  // список отправителей
  std::list<source_ptr> _sources;
  // провайдер
  provider_ptr _provider;
  std::list<std::thread> _threads;
  barier_type barier;
};

///
/// проверки
///

// В любых режимах должно выполняться
// На все запросы отправленны ответы
void check_basic(const toster_config& conf, const source_counters& sc, const receiver_counters& rc)
{
  if ( conf.method_per_request > 0)
  {
    if ( sc.send_counter1 != conf.request_per_thread * conf.requester_threads_per_dst* conf.src_cout  )
      abort();
  }

  if ( conf.method_per_request > 1)
  {
    if ( sc.send_counter2 != conf.request_per_thread * conf.requester_threads_per_dst* conf.src_cout  )
      abort();
  }

  if ( conf.method_per_request > 2)
  {
    if ( sc.send_counter3 != conf.request_per_thread * conf.requester_threads_per_dst* conf.src_cout  )
      abort();
  }

  if ( rc.call_counter1 != rc.callback_counter1 )
    abort();
  if ( rc.call_counter2 != rc.callback_counter2 )
    abort();
  if ( rc.call_counter3 != rc.callback_counter3 )
    abort();
  
  auto call_total = rc.call_counter1 + rc.call_counter2 + rc.call_counter3;
  auto callback_total = rc.callback_counter1 + rc.callback_counter2 + rc.callback_counter3; 
  if ( call_total != callback_total )
    abort();
}

// Проверки для простого режима
void simple_check(const toster_config& conf, const source_counters& sc, const receiver_counters& rc, const provider_counters& pc)
{
  if ( pc.queue_size!=0 )
    abort();
  check_basic(conf, sc, rc);
  auto send_total = sc.send_counter1 + sc.send_counter2 + sc.send_counter3;
  auto  response_total = sc.response_counter1 + sc.response_counter2 + sc.response_counter3;

  std::cout << "send_total = " <<  send_total <<  std::endl;
  std::cout << "response_total=" <<  response_total <<  std::endl;

  if ( send_total != pc.drop_count + response_total)
    abort();
}

void sequence_check(const toster_config& conf, const source_counters& sc, const receiver_counters& rc, const provider_counters& pc)
{
  std::cout << "pc.recall_count=" <<  pc.recall_count <<  std::endl;
  std::cout << "pc.drop_count=" <<  pc.drop_count <<  std::endl;
  std::cout << "pc.orphan_count=" <<  pc.orphan_count <<  std::endl;
  std::cout << "pc.queue_size=" << pc.queue_size <<  std::endl;
  std::cout << "pc.wait_count=" << pc.wait_count <<  std::endl;
  simple_check(conf, sc, rc, pc);
}

///
/// тесты
///

void default_test()
{
  toster_config conf;
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  check_basic(conf, sc, rc);
}

toster_config default_config()
{
  toster_config conf;
  conf.dst_cout = 2;
  conf.src_cout = 3;
  conf.method_per_request = 3;
  conf.request_per_thread = 1000;
  conf.requester_threads_per_dst  = 3;
  conf.request_timeout_ms = 0;
  conf.callback_threads  = 3;
  conf.callback_timeout_ms = 0;
  conf.shutdown_timeout_ms = 0;
  conf.shutdown_time_ms = 0; 
  conf.provider.mode = provider_mode::simple;
  conf.provider.wait_limit = 0;
  conf.provider.wait_warning = 0;
  conf.provider.queue_limit = 0; // 0 - 
  conf.provider.queue_warning = 0;

  return conf;
}

/// 
/// Тесты: mode == simple
///

toster_config simple_config()
{
  auto conf = default_config();
  conf.provider.mode = provider_mode::simple;
  return conf;
}




// Синхронный режим, несколько получателей и отправителей
// без задержек запросов
// без shutdown
void simple_test1()
{
  std::cout << "simple_test1()" << std::endl;
  auto conf = simple_config();
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  if ( pc.drop_count != 0 )
  {
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    abort();
  }

  simple_check(conf, sc, rc, pc);
}

// Acинхронный режим, несколько получателей и отправителей
// без задержек запросов
// без shutdown
void simple_test2()
{
  std::cout << "simple_test2()" << std::endl;
  auto conf = simple_config();
  conf.callback_timeout_ms = 10000;
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  if ( pc.drop_count != 0 )
  {
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    abort();
  }

  simple_check(conf, sc, rc, pc);
}

// cинхронный режим, несколько получателей и отправителей
// с задержкой запросов
// с shutdown
void simple_test3()
{
  std::cout << "simple_test3()" << std::endl;
  auto conf = simple_config();
  conf.request_timeout_ms = 100;
  conf.shutdown_timeout_ms = 10000;
  conf.shutdown_time_ms = 500; // сколько быть в shutdown
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  if ( pc.drop_count == 0 )
  {
    // Плохо настроили, должны быть потери
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    abort();
  }

  simple_check(conf, sc, rc, pc);
}


// Acинхронный режим, несколько получателей и отправителей
// с задержкой запросов
// с shutdown
void simple_test4()
{
  std::cout << "simple_test4()" << std::endl;
  auto conf = simple_config();
  conf.callback_timeout_ms = 10000;
  conf.request_timeout_ms = 100;
  conf.shutdown_timeout_ms = 10000;
  conf.shutdown_time_ms = 500; // сколько быть в shutdown
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  if ( pc.drop_count == 0 )
  {
    // Плохо настроили, должны быть потери
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    abort();
  }
  simple_check(conf, sc, rc, pc);
}


void simple_test()
{
  std::cout << "simple_test()" << std::endl;
  simple_test1();
  simple_test2();
  simple_test3();
  simple_test4();
}

/// 
/// Тесты: mode == sequence
///

toster_config sequence_config()
{
  auto conf = default_config();
  conf.provider.mode = provider_mode::sequenced;
  conf.provider.wait_limit = 1;
  conf.provider.wait_warning = 1;
  conf.provider.queue_limit = 1000000; // 0 - 
  conf.provider.queue_warning = 1000;
  
  return conf;
}


// Синхронный режим, несколько получателей и отправителей
// без задержек запросов
// без shutdown
void sequence_test1()
{
  std::cout << "sequence_test1()" << std::endl;
  auto conf = sequence_config();
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  if ( pc.drop_count != 0 )
  {
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    abort();
  }

  sequence_check(conf, sc, rc, pc);
}

// Acинхронный режим, несколько получателей и отправителей
// без задержек запросов
// без shutdown
void sequence_test2()
{
  std::cout << "sequence_test2()" << std::endl;
  auto conf = sequence_config();
  conf.callback_timeout_ms = 1000;
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  if ( pc.drop_count != 0 )
  {
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    abort();
  }

  sequence_check(conf, sc, rc, pc);
}

// cинхронный режим, несколько получателей и отправителей
// с задержкой запросов
// с shutdown
void sequence_test3()
{
  std::cout << "sequence_test3()" << std::endl;
  auto conf = sequence_config(); 
  conf.src_cout = 1; 
  conf.dst_cout = 1;
  conf.method_per_request = 1;
  conf.requester_threads_per_dst  = 1;
  conf.callback_threads  = 1;

  
  
  conf.provider.wait_timeout_ms = 10000;
  conf.request_timeout_ms = 100;
  conf.shutdown_timeout_ms = 10000;
  conf.shutdown_time_ms = 500; // сколько быть в shutdown
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();

  std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
  std::cout << "pc.recall_count=" << pc.recall_count << std::endl;

  /*
  if ( pc.drop_count == 0 )
  {
    // Плохо настроили, должны быть потери
    abort();
  }
  
  if ( pc.recall_count == 0 )
  {
    abort();
  }*/
  sequence_check(conf, sc, rc, pc);
}


// Acинхронный режим, несколько получателей и отправителей
// с задержкой запросов
// с shutdown
void sequence_test4()
{
  std::cout << "sequence_test4()" << std::endl;
  auto conf = sequence_config();
  conf.provider.wait_timeout_ms = 1000;
  conf.callback_timeout_ms = 100;
  conf.request_timeout_ms = 100;
  conf.shutdown_timeout_ms = 10000;
  conf.shutdown_time_ms = 500; // сколько быть в shutdown
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  /*if ( pc.drop_count == 0 )
  {
    // Плохо настроили, должны быть потери
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    abort();
  }*/
  sequence_check(conf, sc, rc, pc);
}



void sequence_test()
{
  std::cout << "sequence_test()" << std::endl;
  //sequence_test1();
  //sequence_test2();
  sequence_test3();
 // sequence_test4();
}

int main()
{
  // default_test();
  // simple_test();
  sequence_test();
  return 0;
}
