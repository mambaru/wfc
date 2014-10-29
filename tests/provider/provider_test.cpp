#define WFC_DISABLE_GLOBAL_LOG
#define WFC_DISABLE_CLOG

#include <unistd.h>
#include <algorithm>
#include <wfc/provider/provider.hpp>
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
    , null_response_counter1(0)
    , null_response_counter2(0)
    , null_response_counter3(0)
  {}
  
  source_counters(const source_counters& other)
    : send_counter1(other.send_counter1.load())
    , send_counter2(other.send_counter2.load())
    , send_counter3(other.send_counter3.load())
    , response_counter1(other.response_counter1.load())
    , response_counter2(other.response_counter2.load())
    , response_counter3(other.response_counter3.load())
    , null_response_counter1(other.null_response_counter1.load())
    , null_response_counter2(other.null_response_counter2.load())
    , null_response_counter3(other.null_response_counter3.load())
  {}

  void operator += (const source_counters& other)
  {
    send_counter1 += other.send_counter1.load();
    send_counter2 += other.send_counter2.load();
    send_counter3 +=  other.send_counter3.load();
    response_counter1 += other.response_counter1.load();
    response_counter2 += other.response_counter2.load();
    response_counter3 += other.response_counter3.load();
    null_response_counter1 += other.null_response_counter1.load();
    null_response_counter2 += other.null_response_counter2.load();
    null_response_counter3 += other.null_response_counter3.load();
  }
  
  size_t all_send() const
  {
    return send_counter1 + send_counter2 + send_counter3;
  }

  size_t all_response() const 
  {
    return response_counter1 + response_counter2 + response_counter3 
         + null_response_counter1 + null_response_counter2 + null_response_counter3;
  }

  bool ready() const 
  {
    return (this->all_send() == this->all_response());
  }
  
  std::atomic<size_t> send_counter1;
  std::atomic<size_t> send_counter2;
  std::atomic<size_t> send_counter3;
  std::atomic<size_t> response_counter1;
  std::atomic<size_t> response_counter2;
  std::atomic<size_t> response_counter3;
  std::atomic<size_t> null_response_counter1;
  std::atomic<size_t> null_response_counter2;
  std::atomic<size_t> null_response_counter3;
};

struct provider_counters
{
  // Повторные вызовы
  size_t recall_count = 0;
  // Сброшенные по timeout
  size_t drop_count = 0;
  // Потерянные callback's
  size_t orphan_count = 0;
  // размер очереди
  size_t queue_size = 0;
  // размер очереди
  size_t wait_count = 0;
};


struct result_counters
  : provider_counters
{
  result_counters(const provider_counters& pc)
    : provider_counters(pc)
    , call_counter(0)
    , callback_counter(0)
    , send_counter(0)
    , response_counter(0)
    , null_response_counter(0)
  {}
  
  result_counters(const result_counters& other)
    : provider_counters( static_cast<const provider_counters&>(other))
    , call_counter(other.call_counter.load())
    , callback_counter(other.callback_counter.load())
    , send_counter(other.send_counter.load())
    , response_counter(other.response_counter.load())
    , null_response_counter(other.null_response_counter.load())
  {}

  
  void operator+= (const receiver_counters& rc)
  {
    call_counter += (rc.call_counter1 + rc.call_counter2 + rc.call_counter3);
    callback_counter += (rc.callback_counter1 + rc.callback_counter2 + rc.callback_counter3);
  }
  
  void operator+= (const source_counters& sc)
  {
    send_counter += (sc.send_counter1 + sc.send_counter2 + sc.send_counter3);
    response_counter += (sc.response_counter1 + sc.response_counter2 + sc.response_counter3);
    null_response_counter += (sc.null_response_counter1 + sc.null_response_counter2 + sc.null_response_counter3);
  }
  
  void show() const 
  {
    std::cout << "queue_size=" << queue_size << std::endl;
    std::cout << "wait_count=" << wait_count << std::endl;
    std::cout << "recall_count=" << wait_count << std::endl;
    std::cout << "drop_count=" << drop_count << std::endl;
    std::cout << "orphan_count=" << orphan_count << std::endl;
    std::cout << "call_counter=" << call_counter << std::endl;
    std::cout << "callback_counter=" << callback_counter << std::endl;
    std::cout << "send_counter=" << send_counter << std::endl;
    std::cout << "response_counter=" << response_counter << std::endl;
    std::cout << "null_response_counter=" << null_response_counter << std::endl;
  }
  
  void check() const
  {
    if (queue_size != 0)
    {
      this->show();
      abort();
    }

    if (wait_count != 0)
    {
      this->show();
      abort();
    }
    
    if ( call_counter != callback_counter
         || call_counter != response_counter + orphan_count)
    {
      this->show();
      abort();
    }
    
    size_t all_response = response_counter + null_response_counter;
    if (  (send_counter != all_response)
          || (drop_count != null_response_counter)
          
       )
    {
      this->show();
      abort();
    }
  }

  std::atomic<size_t> call_counter;
  std::atomic<size_t> callback_counter;
  std::atomic<size_t> send_counter;
  std::atomic<size_t> response_counter;
  std::atomic<size_t> null_response_counter;

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
        if ( callback )
          callback( std::make_unique<response>(resp) );
      };
      
      if (async_mode) 
      {
        push_(f);
      }
      else 
      {
        f();
      }
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
        if ( callback )
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
        if ( callback )
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

typedef ::wfc::provider::provider_config provider_config;
typedef ::wfc::provider::provider_mode provider_mode;
typedef ::wfc::provider::provider<ireceiver>  provider_type;
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
      //std::cout << "callback resdy" << std::endl;
      if ( resp == nullptr )
      {
        ++(this->null_response_counter1);
        return;
      }
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
      if ( resp == nullptr )
      {
        ++(this->null_response_counter2);
        return;
      }
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
      if ( resp == nullptr )
      {
        ++(this->null_response_counter3);
        return;
      }

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
    //std::cout << "send_all-" << mpr << std::endl;
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


size_t request_count = 10;

///
///
///
struct toster_config
{
  // bool dst_async_mode = false;
  // Кол-во источников
  size_t  dst_cout = 2;
  // Кол-во получателе
  size_t  src_cout = 3;
  // method_per_request=1 только method1, method_per_request=3 все три (method1, method2, method3)  
  size_t method_per_request = 3;
  // Число запросов из потоков
  size_t request_per_thread = request_count;
  // Число потоков делающих запросы
  size_t requester_threads_per_dst  = 3;
  // Таймауты между запросами
  size_t request_timeout_ms = 0;
  // Число потоков активирующие callback на receiver в асинхронном режиме
  size_t callback_threads  = 3;
  // Таймауты между перед отправкой ответа в async_mode
  size_t callback_timeout_ms = 0;
  // таймаут разрыв (0-не используеться). На каждый источнико свой поток
  size_t shutdown_timeout_ms = 0;
  // сколько быть в shutdown
  size_t shutdown_time_ms = 0; 
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
  
  size_t all_send_count() const 
  {
    return conf.src_cout * conf.method_per_request * conf.requester_threads_per_dst * conf.request_per_thread;
  }
  
  toster(const toster_config& conf)
    : conf(conf)
    , ready_time(0)
    , send_ready(0)
  {
    barier.lock();
    // Провайдер
    _provider = std::make_shared<provider_type>(io_service, conf.provider);
    
    // Отправитель
    for (size_t i = 0; i < conf.src_cout; ++i)
    {
      auto src = std::make_shared<source>();
      src->provider = _provider;
      _sources.push_back(src);
      for ( size_t j=0; j < conf.requester_threads_per_dst; ++j)
      {
        ++this->send_ready;
        _threads.push_back( std::thread([this, src]()
        {
          ::wfc::read_lock<barier_type> lk(this->barier);
          //std::cout << "send..." << std::endl;
          for (size_t i=0; i < this->conf.request_per_thread; ++i)
          {
            //std::cout << "send... " << i << std::endl;
            src->send_all(this->conf.method_per_request);
            //std::cout << "send... " << i << " ok"<< std::endl;
            if ( this->conf.request_timeout_ms > 0 )
            {
              usleep(this->conf.request_timeout_ms*1000);
            }
          }
          --this->send_ready;
        }));
      }
    }
    // Получатель
    for (size_t i = 0; i < conf.dst_cout; ++i)
    {
      size_t client_id = i + 1;
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
          while ( dst->callback_one() );
          
          if ( this->conf.callback_timeout_ms > 0 )
          {
            usleep(this->conf.callback_timeout_ms*1000);
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
            usleep(this->conf.shutdown_time_ms*1000);
            this->_provider->startup(client_id, dst);
            usleep(this->conf.shutdown_timeout_ms*1000);
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
    auto sc = this->get_source_counters();
    if ( this->all_send_count() != sc.all_send() )
      return false;
    return sc.ready();
    //return sc.all_send() == sc.all_response();
    /*
    if ( _provider->queue_size() != 0 )
    {
      return false;
    }
    
    if ( this->send_ready != 0 )
      return false;
    time_t now = time(0);
    if ( ready_time == 0 )
    {
      ready_time = now + 1 ;
      return false;
    }
    return now > ready_time;
    */
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
    
    pc.recall_count = _provider->recall_count();
    pc.drop_count = _provider->drop_count();
    pc.orphan_count = _provider->orphan_count();
    pc.queue_size = _provider->queue_size();
    pc.wait_count = _provider->wait_count();
    
    return pc;
  }
  
  result_counters get_result_counters() const
  {
    result_counters rc(this->get_provider_counters());
    rc+=this->get_receiver_counters();
    rc+=this->get_source_counters();
    return rc;
    
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
///
///

void one_test(provider_mode mode, bool async, bool shutdown)
{
  toster_config conf;
  conf.provider.mode = mode;
  if (async)
  {
    conf.callback_threads  = 3;
    conf.callback_timeout_ms = 5;
    conf.provider.wait_timeout_ms = 5;
  }
  
  if (shutdown)
  {
    conf.request_timeout_ms = 1;
    conf.shutdown_timeout_ms = 10;
    conf.shutdown_time_ms = 1; 
  }

  toster t(conf);
  t.run();
  auto rc = t.get_result_counters();
  rc.check();
}

void mode_test(provider_mode mode)
{
  std::cout << " -------- sync, no-shutdown --------" << std::endl;
  one_test(mode, false, false);
  std::cout << " -------- async, no-shutdown --------" << std::endl;
  one_test(mode, true,  false);
  std::cout << " -------- sync, shutdown --------" << std::endl;
  one_test(mode, false, true);
  std::cout << " -------- async, shutdown --------" << std::endl;
  one_test(mode, true, true);
}

void all_tests()
{
  std::cout << "================ simple ================" << std::endl;
  mode_test(provider_mode::simple);
  std::cout << "================ connectify ================" << std::endl;
  mode_test(provider_mode::connectify);
  std::cout << "================ insured ================" << std::endl;
  mode_test(provider_mode::insured);
  std::cout << "================ sequenced ================" << std::endl;
  mode_test(provider_mode::sequenced);
}




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
  auto  null_response_total = sc.null_response_counter1 + sc.null_response_counter2 + sc.null_response_counter3;

  std::cout << "send_total = " <<  send_total <<  std::endl;
  std::cout << "response_total=" <<  response_total <<  std::endl;
  std::cout << "pc.drop_count=" <<  pc.drop_count <<  std::endl;

  if ( send_total != response_total)
    abort();
  if ( pc.drop_count != null_response_total)
    abort();

  // Все профукали, надо поднастроить
  if ( response_total == null_response_total)
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
  std::cout << "default_test()" << std::endl;
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
  conf.callback_threads  = 0;
  conf.callback_timeout_ms = 0;
  conf.shutdown_timeout_ms = 0;
  conf.shutdown_time_ms = 0; 
  conf.provider.mode = provider_mode::simple;
  conf.provider.max_waiting = 0;
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


toster_config insured_config()
{
  auto conf = default_config();
  conf.provider.mode = provider_mode::insured;
  conf.provider.max_waiting = 1;
  conf.provider.queue_limit = 1000000; // 0 - 
  conf.provider.queue_warning = 1000;
  
  return conf;
}


// Синхронный режим, несколько получателей и отправителей
// без задержек запросов
// без shutdown
void insured_test1()
{
  std::cout << "insured_test1()" << std::endl;
  auto conf = insured_config();
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
void insured_test2()
{
  std::cout << "insured_test2()" << std::endl;
  auto conf = insured_config();
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
void insured_test3()
{
  std::cout << "insured_test3()" << std::endl;
  auto conf = insured_config(); 
  conf.provider.mode = provider_mode::insured;
  conf.provider.max_waiting = 32;
  conf.provider.wait_timeout_ms = 30000;
  conf.request_timeout_ms = 100;
  conf.shutdown_timeout_ms = 10000;
  conf.shutdown_time_ms = 50; // сколько быть в shutdown
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();

  std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
  std::cout << "pc.recall_count=" << pc.recall_count << std::endl;

  
  if ( pc.recall_count == 0 )
  {
    // Плохо настроили, должны быть повторные вызовы
    abort();
  }
  sequence_check(conf, sc, rc, pc);
}


// Acинхронный режим, несколько получателей и отправителей
// с задержкой запросов
// с shutdown
void insured_test4()
{
  std::cout << "insured_test4()" << std::endl;
  auto conf = insured_config();
  conf.provider.wait_timeout_ms = 1000;
  conf.callback_timeout_ms = 100;
  conf.request_timeout_ms = 100;
  conf.shutdown_timeout_ms = 10000;
  conf.shutdown_time_ms = 1000; // сколько быть в shutdown
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
  sequence_check(conf, sc, rc, pc);
}



void insured_test()
{
  std::cout << "insured_test()" << std::endl;
  insured_test1();
  insured_test2();
  insured_test3();
  insured_test4();
}

/// 
/// Тесты: mode == sequence
///

toster_config sequence_config()
{
  auto conf = default_config();
  conf.provider.mode = provider_mode::sequenced;
  conf.provider.max_waiting = 1;
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

  
  if ( pc.recall_count == 0 )
  {
    // Плохо настроили, должны быть повторные вызовы
    abort();
  }
  sequence_check(conf, sc, rc, pc);
}


// Acинхронный режим, несколько получателей и отправителей
// с задержкой запросов
// с shutdown
void sequence_test4()
{
  std::cout << "sequence_test4()" << std::endl;
  auto conf = sequence_config();
  //conf.request_per_thread = 1;
  //conf.method_per_request = 1;
  
  conf.provider.wait_timeout_ms = 1;
  conf.callback_timeout_ms = 2;
  conf.request_timeout_ms = 1;
  //conf.shutdown_timeout_ms = 10000;
  //conf.shutdown_time_ms = 10000; // сколько быть в shutdown
  toster t(conf);
  t.run();
  auto sc = t.get_source_counters();
  auto rc = t.get_receiver_counters();
  auto pc = t.get_provider_counters();
  if ( pc.drop_count == 0 )
  {
    // Плохо настроили, должны быть потери
    std::cout << "pc.drop_count=" << pc.drop_count << std::endl;
    //abort();
  }
  sequence_check(conf, sc, rc, pc);
}



void sequence_test()
{
  std::cout << "sequence_test()" << std::endl;
  //sequence_test1();
  //sequence_test2();
  //sequence_test3();
  sequence_test4();
}

int main(int argc, char* argv[])
{
  if ( argc>1 )
  {
    if ( int count = atoi(argv[1]) )
    {
      request_count = count;
    }
  }
  all_tests();
  //default_test();
  //simple_test();
  //sequence_test();
  //insured_test();
  return 0;
}
