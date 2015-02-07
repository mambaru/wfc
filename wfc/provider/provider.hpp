#pragma once

#include <wfc/provider/basic_provider.hpp>
#include <wfc/provider/provider_config.hpp>
#include <wfc/memory.hpp>
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

#include <fas/integral/int_.hpp>

#include <boost/asio.hpp>
#include <memory>
#include <deque>
#include <list>
#include <thread>
#include <map>
#include <set>

#define PROVIDER_SHOW_COUNTERS(pthis) ""\
  << ". post: " << pthis->_post_count << ", confirm: "<< pthis->_confirm_count << ", bad_gateway: " << pthis->_bad_gateway_count          \
  << ", queue: " << pthis->queue_size_() << ", wait: "<< pthis->wait_count_()          \
  << ", drop_total: " << pthis->drop_count_() << ", drop_timeout: " << pthis->_drop_count_wait << ", drop_limit: " << pthis->_drop_count_limit \
  << ",  recall: " << pthis->_recall_count << ",  orphan: " << pthis->_orphan_count << ",  orphan_deadline: " << pthis->_orphan_deadline


namespace wfc{ namespace provider{ 
  
class wait_map
{
  typedef std::mutex mutex_type;
  //const size_t null_id = static_cast<size_t>(-1);
public:
  
  typedef size_t client_id_t;
  typedef size_t call_id_t;
  //typedef std::pair<client_id_t, call_id_t> clicall_pair;
  
  typedef ::boost::asio::deadline_timer deadline_timer;
  typedef std::shared_ptr<deadline_timer> timer_ptr;
  struct info;
  typedef std::shared_ptr<info> info_ptr;
  typedef std::function<info_ptr()> post_function;
  
  struct info
  {
    client_id_t client_id;
    call_id_t   call_id;
    post_function post_fun;
    timer_ptr timer;
  };
  
  struct by_call_cli
  {
    bool operator()( const info_ptr& left,  const wait_map::info_ptr& right) const
    {
      /*if ( !(left->call_id < right->call_id) )
        return false;
      return left->client_id < right->client_id;
      */
      return left->call_id < right->call_id;
    }
  };

  struct by_cli_call
  {
    bool operator()( const info_ptr& left,  const info_ptr& right)  const
    {
      if ( left->client_id < right->client_id )
        return true;
      if ( left->client_id > right->client_id )
        return false;
      return left->call_id < right->call_id;
      /*
      if ( !(left->client_id  < right->client_id ) )
        return false;
      return left->call_id < right->call_id;
      */
    }
  };

  info_ptr erase_call(call_id_t call_id)
  {
    auto k = std::make_shared<info>(info{0, call_id, nullptr, nullptr});
    auto itr = _call_cli.find(k);
    if ( itr == _call_cli.end() )
      return nullptr;
    k = *itr;
    _call_cli.erase(k);
    _cli_call.erase(k);
    this->check_();
    return k;
    
  }
  
  std::vector<info_ptr> erase_client(client_id_t client_id)
  {
    std::vector<info_ptr> result;
    auto k = std::make_shared<info>(info{client_id, 0, nullptr, nullptr});
    auto beg = _cli_call.lower_bound(k);
    k->client_id += 1;
    auto end = _cli_call.lower_bound(k);
    result.reserve( std::distance(beg, end) );
    auto itr = beg;
    for (; itr!=end; )
    {
      auto itr2 = _call_cli.find( *itr );
      if ( itr2 != _call_cli.end() )
      {
        result.push_back( *itr );
        _call_cli.erase(itr2);
        _cli_call.erase(itr++);
        
      }
      else
      {
        DAEMON_LOG_FATAL("Ошибка логики. В списке ожидания не найден объект по call_id=" 
                    << (*itr)->call_id << ". wfc::provider::shutdown client_id=" << client_id )
        abort();
      }
    }
    this->check_();
    
    /*
    
    auto beg = _clicall.lower_bound({client_id, 0});
    auto end = _clicall.upper_bound({client_id, null_id});
    result.reserve( std::distance(beg, end) );
    auto itr = beg;
    for (; itr!=end; ++itr)
    {
      if ( itr->first != client_id )
        break;
      auto itr2 = _callclipost.find( itr->second );
      if ( itr2 != _callclipost.end() )
      {
        result.push_back(itr2->second);
      }
      else
      {
        DAEMON_LOG_FATAL("Ошибка логики. В списке ожидания не найден объект по call_id=" 
                    << itr->second << ". wfc::provider::shutdown client_id=" << client_id )
        abort();
      }
    }
    _clicall.erase(beg, itr);
    */
    return std::move(result);
  }
  
  info_ptr insert(client_id_t client_id, call_id_t call_id )
  {
    
    auto k = std::make_shared<info>(info{client_id, call_id, nullptr, nullptr});
    if ( !_cli_call.insert(k).second)
    {
      DAEMON_LOG_FATAL("Ошибка логики. Пара client_id+call_id существует");
      abort();
    }

    if ( !_call_cli.insert(k).second)
    {
      DAEMON_LOG_FATAL("Ошибка логики. Пара call_id+client_id существует");
      abort();
    }
     return k;  
  }
  
  size_t size() const
  {
    this->check_();
    return _cli_call.size();
  }
  
private:
  void check_() const
  {
    if ( _cli_call.size() != _call_cli.size() )
    {
      DAEMON_LOG_FATAL("provider::wait_count_: index error. _clicall.size()=" << _cli_call.size() << " _callclipost.size()" << _call_cli.size())
      abort();
    }
  }
  
private:
  typedef std::set<info_ptr, by_call_cli> index_by_call_cli;
  typedef std::set<info_ptr, by_cli_call> index_by_cli_call;
  
  index_by_call_cli _call_cli; 
  index_by_cli_call _cli_call;
  /*
  typedef std::map<call_id_t, info> callclipost_map;
  typedef std::set<clicall_pair> clicall_set;
  callclipost_map _callclipost;
  clicall_set _clicall;
  */
  //mutable mutex_type _mutex;
};


template<typename Itf>  
class provider
  : public basic_provider<Itf, provider, std::recursive_mutex>
{
  typedef provider<Itf> self;
  typedef basic_provider<Itf, provider, std::recursive_mutex> super;
  typedef std::chrono::high_resolution_clock clock_t;
  typedef clock_t::time_point time_point_t;

  //const size_t null_id = static_cast<size_t>(-1);

public:
  typedef ::wfc::asio::io_service io_service_type;

  typedef wait_map::info wait_info;
  typedef wait_map::info_ptr info_ptr;
  
  typedef typename super::interface_type interface_type;
  typedef typename super::mutex_type mutex_type;
  //typedef std::pair<size_t, size_t> clicall_pair;
  typedef std::function<info_ptr()> post_function;
  typedef ::boost::asio::deadline_timer deadline_timer;
  typedef std::shared_ptr<deadline_timer> timer_ptr;
  typedef std::tuple<size_t, post_function, timer_ptr> clipost;
  //typedef std::map<size_t, clipost> callclipost_map;
  //typedef std::set<clicall_pair> clicall_set;
  typedef std::deque< post_function > delayed_queue;
  
  ~provider()
  {
    if ( this->_io_service_ptr!=nullptr )
    {
      this->_io_service_ptr->stop();
      for( auto& t: _threads)
      {
        t.join();
      }
    }
  }

  provider( io_service_type& io, const provider_config& conf)
    : super(conf)
    , _io_service(io)
    , _io_work(io)
    , _call_id_counter(0)
    , _post_count(0)
    , _confirm_count(0)
    , _recall_count(0)
    , _orphan_count(0)
    , _orphan_deadline(0)
    , _drop_count_wait(0)
    , _drop_count_limit(0)
    , _bad_gateway_count(0)
    , _logtm(0)
  {
    if ( conf.threads > 0 )
    {
      this->_io_service_ptr = std::make_shared< io_service_type >();
      for (int i = 0; i < conf.threads; ++i)
      {
        _threads.push_back(std::thread([this](){
          io_service_type::work wrk( *this->_io_service_ptr);
          this->_io_service_ptr->run();
        }));
      }
    }
  }
  
  io_service_type& get_io_service() const
  {
    if ( _io_service_ptr != nullptr )
      return *_io_service_ptr;
    return _io_service;
  }
  
  size_t post_count() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _post_count;
  }

  size_t orphan_count() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _orphan_count;
  }

  size_t recall_count() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _recall_count;
  }

  size_t drop_count() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return drop_count_();
  }

  // Размер очереди
  size_t queue_size() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return queue_size_();
  }

  // Число запросов ожидающих подверждения
  size_t wait_count() const
  {
    return this->wait_count_();
  }


  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    super::startup_(client_id, ptr);
    DAEMON_LOG_MESSAGE("::wfc::provider::startup new connecion with client_id=" << client_id << " queue size=" << _queue.size() );
    this->process_queue_();
  }

  virtual void shutdown(size_t client_id)
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    super::shutdown_(client_id);
    auto erased = _wait_map.erase_client(client_id);
    DAEMON_LOG_MESSAGE("::wfc::provider::shutdown connecion with client_id=" << client_id << " queue size=" << _queue.size() << " waits for =" << erased.size() );
    for (auto& e : erased)
    {
      if ( auto f = e->post_fun )
      {
        _queue.push_front( f );
        ++_recall_count; // !!!Счетчик будующих повторных вызовов
        this->log_if_( log::recall_warn);
      }
      else
      {
        // Может и не аборт 
        DAEMON_LOG_FATAL("Ошибка логики. Нулевой запрос по call_id=X" /*<< e->second*/ << ". wfc::provider::shutdown client_id=" << client_id )
        abort();
      }
    }

    /*
    auto beg = _clicall.lower_bound({client_id, 0});
    auto end = _clicall.upper_bound({client_id, null_id});
    for (auto itr = beg; itr!=end; )
    {
      if ( itr->first != client_id )
        break;
      auto itr2 = _callclipost.find( itr->second );
      if ( itr2 != _callclipost.end() )
      {
        if ( auto f = std::get<1>(itr2->second) )
        {
          // Дупустимо переполнение очереди на wait_limit
          _queue.push_front( std::get<1>(itr2->second) );
          _callclipost.erase(itr2);
          _clicall.erase(itr++);
          this->wait_check_();
        }
        else
        {
          DAEMON_LOG_FATAL("Ошибка логики. Нулевой запрос по call_id=" 
                    << itr->second << ". wfc::provider::shutdown client_id=" << client_id )
          abort();
          
        }
      }
      else
      {
        DAEMON_LOG_FATAL("Ошибка логики. В списке ожидания не найден объект по call_id=" 
                    << itr->second << ". wfc::provider::shutdown client_id=" << client_id )
        abort();
      }

      ++_recall_count; // !!!Счетчик будующих повторных вызовов
      this->log_if_( log::recall_warn);
    }
    */
    this->process_queue_();
  }
  
  template<typename R, typename Resp, typename ... Args> 
  void callback_null_(const std::function<R(Resp, Args ...)>& callback)
  {
    if ( callback!=nullptr)
    {
      callback( nullptr, Args()... );
    }
  }
  
  template<typename Req, typename Callback, typename... Args>
  void post(
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    this->post_<1>( mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)...);
  }

  template<typename Req, typename OrigCallback, typename Callback, typename... Args>
  void post_ex(
    void (interface_type::*mem_ptr)(Req, OrigCallback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    this->post_<2>( mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)...);
  }
  
private:
  
  enum class log
  {
    drop_timeout,
    drop_limit,
    limit_warn,
    recall_warn,
    bad_gateway
  };
  
  void log_if_(log type, bool logtm = false)
  {
    
    if (logtm)
    {
      time_t now = time(0);
      if ( !(_logtm < now) )
        return;
      _logtm = now;
    }
    
    switch(type)
    {
      case log::drop_timeout:
        DAEMON_LOG_WARNING("wfc::provider: lost message. Drop by wait timeout. wait_timeout_ms=" << super::_conf.wait_timeout_ms << PROVIDER_SHOW_COUNTERS(this) )
        break;
      case log::drop_limit:
        DAEMON_LOG_WARNING("wfc::provider: lost message. Drop by queue_limit. queue_limit=" << super::_conf.queue_limit << PROVIDER_SHOW_COUNTERS(this) )
        break;
      case log::limit_warn:
        DAEMON_LOG_WARNING("wfc::provider: queue size limit warning. queue_warning=" << super::_conf.queue_warning << PROVIDER_SHOW_COUNTERS(this))
        break;
      case log::recall_warn:
        DAEMON_LOG_WARNING("wfc::provider: connection was closed while waiting for response. When you restore connection will be re-sending a message" << PROVIDER_SHOW_COUNTERS(this) )
      case log::bad_gateway:
        DAEMON_LOG_WARNING("wfc::provider: bad gateway. Re-sending a message" << PROVIDER_SHOW_COUNTERS(this) )
        break;
    }
  }
  
  size_t queue_size_() const
  {
    return _queue.size();
  }

  size_t wait_count_() const
  {
    return _wait_map.size();
    /*
    this->wait_check_();
    return _clicall.size();
    */
  }

  /*
  void wait_check_() const
  {
    if ( _clicall.size() != _callclipost.size() )
    {
      DAEMON_LOG_FATAL("provider::wait_count_: index error. _clicall.size()=" << _clicall.size() << " _callclipost.size()" << _callclipost.size())
      abort();
    }
  }
  */
  
  size_t drop_count_() const
  {
    return _drop_count_wait + _drop_count_limit;
  }

  
private:

  template<int N, typename Req, typename OrigCallback, typename Callback, typename... Args>
  void post_( 
    void (interface_type::*mem_ptr)(Req, OrigCallback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    
    if ( !super::_conf.enabled )
    {
      callback_null_( callback );
      return;
    }

    std::lock_guard<mutex_type> lk( super::_mutex );
    
    if ( super::_conf.max_waiting==0  )
    {
      size_t client_id = 0;
      // Простой режим
      if ( auto cli = super::get_(client_id) )
      {
        size_t call_id = ++this->_call_id_counter;
        (cli.get()->*mem_ptr)( 
          std::move(req), 
          this->wrap_callback_<OrigCallback>( 
            fas::int_<N>(), 
            client_id, 
            call_id, 
            callback 
          ), 
          std::forward<Args>(args)... 
        );
        return;
      }
      else if (super::_conf.queue_limit==0)
      {
        callback_null_( callback );
        ++_drop_count_limit;
        this->log_if_( log::drop_limit, true);
        return;
      }
    }
    
    // Далее нужна обертка
    post_function f = this->wrap_<N>(mem_ptr, std::move(req), callback, std::forward<Args>(args)...); 
    
    if ( _wait_map.size() < super::_conf.max_waiting  )
    {
      if ( 0 != super::ready_count_() )
      {
        auto result = f();
        if ( this->update_waiting_(result, f) )
          return;
      }
      else
      {
        DAEMON_LOG_MESSAGE("wfc::provider: No connection. The request will be sent to the queue.")
      }
    }
    
    if ( _queue.size() < super::_conf.queue_limit )
    {
      _queue.push_back( f );
      if ( _queue.size() > super::_conf.queue_warning )
        this->log_if_( log::limit_warn, true);
    }
    else
    {
      callback_null_( callback );
      ++_drop_count_limit;
      this->log_if_(log::drop_limit, true);
    }
  }


  
  template<int N, typename Req, typename OrigCallback, typename Callback, typename... Args>
  static info_ptr send_( 
    std::shared_ptr<self> pthis,
    void (interface_type::*mem_ptr)(Req, OrigCallback, Args... args), 
    std::shared_ptr<Req> req, 
    const Callback& callback, 
    Args... args
  )
  {
    
    
    size_t call_id = ++pthis->_call_id_counter;
    
    // clicall_pair result = {pthis->null_id, call_id};
    info_ptr inf = nullptr;
    size_t client_id = 0;
    if ( auto cli = pthis->get_( client_id /*result.first*/ ) )
    {
      OrigCallback wcallback = pthis->wrap_callback_<OrigCallback>( fas::int_<N>(), client_id, call_id, callback);
      
      //std::shared_ptr<deadline_timer> timer;
      //info_ptr inf;
      std::function<void(const ::boost::system::error_code&)> timer_fun;
      
      if (pthis->_conf.max_waiting != 0 )
      {
        inf = pthis->_wait_map.insert(/*result.first*/client_id, call_id);
        if (pthis->_conf.wait_timeout_ms != 0 )
        {
          inf->timer = std::make_shared<deadline_timer>(pthis->get_io_service());
        }
        
        /*
        if ( !pthis->_clicall.insert({result.first, call_id}).second )
        {
          DAEMON_LOG_FATAL("wfc::provider: insert to set fail! client_id=" << result.first << " call_id=" << call_id)
          abort();
        }
        auto respair = pthis->_callclipost.insert({call_id, clipost{result.first, nullptr, nullptr}});
        if ( !respair.second )
        {
          DAEMON_LOG_FATAL("wfc::provider: insert to map fail! client_id=" << result.first << " call_id=" << call_id)
          abort();
        }
        pthis->wait_check_();
        
        auto call_itr = respair.first;
        
        if (pthis->_conf.wait_timeout_ms != 0 )
        {
          timer = std::make_shared<deadline_timer>(pthis->_io_service);
          std::get<2>(call_itr->second) = timer;
        }
        */
      }
      
      try
      {
        if (pthis->_conf.max_waiting != 0 )
        {
          // Копируем запрос, т.к. возможен повторный вызов
          auto req_for_send = std::make_unique<typename Req::element_type>(**req);
          if (inf->timer)
          {
            //auto client_id = result.first;
            timer_fun = [pthis, client_id, call_id, callback]( const ::boost::system::error_code& ec)
            {
              if ( ec == boost::asio::error::operation_aborted )
                return;
              self::mt_deadline_<Callback>(pthis, client_id, call_id, std::move(callback));
            };
          }
          ++(pthis->_post_count);
          (cli.get()->*mem_ptr)( std::move(req_for_send), std::move(wcallback), std::forward<Args>(args)... );
        }
        else
        {
          ++(pthis->_post_count);
          (cli.get()->*mem_ptr)( std::move(*req), std::move(wcallback), std::forward<Args>(args)... );
        }
      } 
      catch ( ... )
      {
        DAEMON_LOG_ERROR("provider::send_: unhandled exception")
        throw;
      }
      
      if ( inf!=nullptr &&  inf->timer!=nullptr )
      {
        inf->timer->expires_from_now(boost::posix_time::milliseconds(pthis->_conf.wait_timeout_ms));
        inf->timer->async_wait( timer_fun );
      }
    }
    else
    {
      DAEMON_LOG_WARNING("provider::send_: соединение потеряно")
    }
    return inf;
  }
  
  template<typename Resp, typename... Args>
  static void mt_confirm_( 
    std::shared_ptr<self> pthis,
    size_t call_id,
    const std::function<void(Resp, Args...)>& callback,
    std::shared_ptr<Resp> resp, 
    Args... args
  )
  {
    // TODO: if *resp == nullptr значит был bad_gateway, т.е. нужно перепослать
    {
      std::lock_guard<mutex_type> lk(pthis->_mutex); 
      
      if ( auto inf = pthis->_wait_map.erase_call(call_id) )
      {
        if ( *resp != nullptr )
        {
          ++pthis->_confirm_count;
        }
        else
        {
          ++pthis->_bad_gateway_count;
          pthis->_queue.push_front( inf->post_fun );
          pthis->log_if_(log::bad_gateway);
        }
        
        if ( inf->timer )
        {
          inf->timer->cancel();
          inf->timer=nullptr;
        }
       
        pthis->process_queue_();
      }
      else
      {
        ++pthis->_orphan_count;
        return;
      }
      
      
      /*
      auto call_itr = pthis->_callclipost.find(call_id);
      if ( call_itr == pthis->_callclipost.end() )
      {
        ++pthis->_orphan_count;
        return;
      }
      
      if ( *resp != nullptr )
      {
        ++pthis->_confirm_count;
      }
      else
      {
        ++pthis->_bad_gateway_count;
        pthis->_queue.push_front( std::get<1>(call_itr->second) );
        pthis->log_if_(log::bad_gateway);
      }
      
      if ( std::get<2>(call_itr->second) )
      {
        std::get<2>(call_itr->second)->cancel();
        std::get<2>(call_itr->second)=nullptr;
      }
      pthis->_callclipost.erase(call_itr);
      pthis->_clicall.erase({std::get<0>(call_itr->second),call_id});
      pthis->wait_check_();
      pthis->process_queue_();
      */
    }
      
    if ( callback!=nullptr )
      callback( std::move(*resp), std::forward<Args>(args)... );
  }


  template<typename Callback>
  static void mt_deadline_( 
    std::shared_ptr<self> pthis,
    size_t /*client_id*/, 
    size_t call_id,
    const Callback& callback
  )
  {
    std::lock_guard<mutex_type> lk( pthis->_mutex );
    
    if ( auto inf = pthis->_wait_map.erase_call(call_id) )
    {
      ++pthis->_drop_count_wait;
      pthis->log_if_( log::drop_timeout );
      pthis->callback_null_( std::move(callback) );
      pthis->process_queue_();
    }
    else
    {
      ++pthis->_orphan_deadline;
      return;
    }
      

    /*
    auto call_itr = pthis->_callclipost.find(call_id);
    if ( call_itr == pthis->_callclipost.end() )
    {
      ++pthis->_orphan_deadline;
      return;
    }
    
    auto cli_itr = pthis->_clicall.find( { std::get<0>(call_itr->second), call_itr->first} );
    if ( cli_itr == pthis->_clicall.end() )
    {
      DAEMON_LOG_FATAL("provider::mt_deadline_: ошибка логики")
      abort();
    }

    if ( client_id != cli_itr->first )
    {
      DAEMON_LOG_FATAL("provider::mt_deadline_: ошибка логики")
      abort();
      return;
    }
    
    ++pthis->_drop_count_wait;
    pthis->log_if_( log::drop_timeout );

    pthis->callback_null_( std::move(callback) );
    pthis->_callclipost.erase(call_itr);
    pthis->_clicall.erase(cli_itr);
    pthis->wait_check_();
    */
    //pthis->process_queue_();
  }
  
  template<int N, typename Req, typename OrigCallback, typename Callback, typename... Args>
  post_function wrap_( 
    void (interface_type::*mem_ptr)(Req, OrigCallback, Args... args), 
    Req req, 
    const Callback& callback, 
    Args... args
  )
  {
    auto preq = std::make_shared<Req>( std::move(req) );
    auto pthis = super::shared_from_this();
    
    return std::bind(
      &provider<Itf>::send_<N, Req, OrigCallback, Callback, Args...>,
      pthis,
      mem_ptr, 
      preq, 
      callback,
      std::forward<Args>(args)...
    );
  }
  
  
  template<typename OrigCallback, typename Resp, typename ...Args>
  OrigCallback
  wrap_callback_(
    fas::int_<1>, 
    size_t /*client_id*/, 
    size_t call_id, 
    const std::function<void(Resp, Args...)>& callback
  ) 
  {
    if ( super::_conf.max_waiting == 0 )
      return std::move(callback);
    auto pthis = this->shared_from_this();
    auto callback_fun = &provider<Itf>::mt_confirm_<Resp, Args...>;
    return [callback_fun, pthis, call_id, callback](Resp resp, Args... args)
    {
      auto presp = std::make_shared<Resp>(std::move(resp) );
      pthis->get_io_service().post( std::bind(
        callback_fun,
        pthis,
        call_id,
        callback,
        presp,
        std::forward<Args>(args)...
      ) );
    };
  }

  template<typename OrigCallback, typename Resp, typename ...Args>
  OrigCallback
  wrap_callback_(
    fas::int_<2>, 
    size_t client_id, 
    size_t call_id, 
    std::function<void(Resp, size_t, Args...)> callback
  ) 
  {
    if ( super::_conf.max_waiting == 0 )
    {
      if (callback==nullptr)
        return nullptr;
      
      return [client_id, callback](Resp resp, Args... args)
      {
        callback( std::move(resp), client_id, std::forward<Args>(args)...);
      };
    }
    
    auto pthis = this->shared_from_this();
    
    auto callback_fun = &provider<Itf>::mt_confirm_<Resp, size_t, Args...>;
    
    return [callback_fun, pthis, client_id, call_id, callback](Resp resp, Args... args)
    {
      auto presp = std::make_shared<Resp>(std::move(resp) );
      pthis->get_io_service().post( std::bind(
        callback_fun,
        pthis,
        call_id,
        callback,
        presp,
        client_id, std::forward<Args>(args)...
      ) );
    };
  }

  bool update_waiting_(info_ptr inf, const post_function& f)
  {
    if ( super::_conf.max_waiting == 0 )
      return true;
    
    if ( inf == nullptr )
      return false;
    
    inf->post_fun = std::move(f);
    
    return true;
  }
  
  /*bool update_waiting_(const clicall_pair& result, const post_function& f)
  {
    if ( result.first == null_id )
      return false;
    
    if ( super::_conf.max_waiting != 0 )
    {
      auto itr = _callclipost.find(result.second);
      if ( itr != _callclipost.end() )
      {
        std::get<1>(itr->second) = std::move(f);
      }
      else
      {
        DAEMON_LOG_FATAL("provider::process_queue_: ошибка логики")
        abort();
      }
    }
    return true;
  }
  */
  int process_queue_()
  {
    int count = 0;
    while ( !_queue.empty() )
    {
      
      if ( super::_conf.max_waiting!=0 && _wait_map.size() >= super::_conf.max_waiting )
        break;

      auto f = _queue.front();
      if ( f == nullptr )
      {
        DAEMON_LOG_FATAL("provider::process_queue_: ошибка логики")
        abort();
      }
      
      auto result = f();
      
      if ( !this->update_waiting_(result, f) )
        break;
      
      _queue.pop_front();
      
      if ( _queue.size() > super::_conf.queue_warning )
        this->log_if_( log::limit_warn, true);

    }
    return count;
  }

private:
  
  io_service_type& _io_service;
  std::shared_ptr< io_service_type > _io_service_ptr;
  std::list< std::thread > _threads;
  io_service_type::work _io_work;
  size_t _call_id_counter;
  // callclipost_map _callclipost;
  // clicall_set _clicall;
  delayed_queue _queue;
  
  size_t _post_count;
  size_t _confirm_count;
  // Повторные вызовы
  size_t _recall_count;
  // Потерянные вызовы (ответы пришедшие после recall или когда отвалились по timeout)
  size_t _orphan_count;
  size_t _orphan_deadline; // Удалить, нафиг не нужно. Для отладки
  // сброшенные по timeout
  size_t _drop_count_wait;
  size_t _drop_count_limit;
  size_t _bad_gateway_count;
  
  time_t _logtm;
  
  wait_map _wait_map;
};

}}
