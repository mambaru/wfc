#pragma once

#include <wfc/provider/basic_provider.hpp>
#include <wfc/provider/provider_config.hpp>
#include <wfc/memory.hpp>
#include <wfc/logger.hpp>
#include <wfc/io_service.hpp>

#include <fas/integral/int_.hpp>

#include <boost/asio.hpp>
#include <memory>
#include <deque>
#include <map>
#include <set>

#define PROVIDER_SHOW_COUNTERS(pthis) std::endl                                     \
  << "post: " << pthis->_post_count << ", confirm: "<< pthis->_confirm_count        \
  << ", drop_wait: " << pthis->_drop_count_wait << ", drop_limit: " << pthis->_drop_count_limit        \
  << ",  recall: " << pthis->_recall_count << ",  orphan: " << pthis->_orphan_count 
 

namespace wfc{ namespace provider{ 

  /*
template<typename Callback>
struct callback_type_1

  
  template<typename Resp, typename ...Args>
  struct callback_type_1
  {
    typedef std::function<void(Resp, Args...)> type;
  };
  */

  
template<typename Itf>  
class provider
  : public basic_provider<Itf, provider, std::recursive_mutex>
{
  typedef provider<Itf> self;
  typedef basic_provider<Itf, provider, std::recursive_mutex> super;
  typedef std::chrono::high_resolution_clock clock_t;
  typedef clock_t::time_point time_point_t;

  const size_t null_id = static_cast<size_t>(-1);

public:
  
  typedef typename super::interface_type interface_type;
  typedef typename super::mutex_type mutex_type;
  typedef std::pair<size_t, size_t> clicall_pair;
  typedef std::function<clicall_pair()> post_function;
  typedef ::boost::asio::deadline_timer deadline_timer;
  typedef std::shared_ptr<deadline_timer> timer_ptr;
  typedef std::tuple<size_t, post_function, timer_ptr> clipost;
  typedef std::map<size_t, clipost> callclipost_map;
  
  typedef std::set<clicall_pair> clicall_set;
  typedef std::deque< post_function > delayed_queue;

  provider( ::wfc::io_service& io, const provider_config& conf)
    : super(conf)
    , _io_service(io)
    , _io_work(io)
    , _call_id_counter(0)
    , _post_count(0)
    , _confirm_count(0)
    , _recall_count(0)
    , _orphan_count(0)
    , _drop_count_wait(0)
    , _drop_count_limit(0)
  {
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

  size_t drop_count_() const
  {
    return _drop_count_wait + _drop_count_limit;
  }

  // Размер очереди
  size_t queue_size() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _queue.size();
  }

  // Число запросов ожидающих подверждения
  size_t wait_count() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _clicall.size();
  }

  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    super::startup_(client_id, ptr);
    this->process_queue_();
  }

  virtual void shutdown(size_t client_id)
  {
    std::lock_guard<mutex_type> lk( super::_mutex );

    super::shutdown_(client_id);

    auto beg = _clicall.lower_bound({client_id, 0});
    auto end = _clicall.upper_bound({client_id, null_id});
    for (auto itr = beg; itr!=end; )
    {
      if ( itr->first != client_id )
        break;
      auto itr2 = _callclipost.find( itr->second );
      if ( itr2 != _callclipost.end() )
      {
        // Дупустимо переполнение очереди на wait_limit
        _queue.push_front( std::get<1>(itr2->second) );
        _callclipost.erase(itr2);
        _clicall.erase(itr++);
      }
      else
      {
        itr++;
        continue;
      }

      ++_recall_count; // !!!Счетчик будующих повторных вызовов
      DAEMON_LOG_WARNING( 
        "provider: закрытие соединения при ожидании подверждения удаленного вызова. "
        "Информация о выполнении вызова отсутствует. При востановлении соединения будет сделан повторный вызов. "
        "Всего повторных вызовов " << this->_recall_count << 
        PROVIDER_SHOW_COUNTERS(this)
      )
    }
    this->process_queue_();
  }
  
  template<typename R, typename Resp, typename ... Args> 
  void callback_null_(std::function<R(Resp, Args ...)>&& callback)
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
      callback_null_( std::move(callback) );
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
        (cli.get()->*mem_ptr)( std::move(req), this->wrap_callback_<OrigCallback>( fas::int_<N>(), client_id, call_id, callback ), std::forward<Args>(args)... );
        return;
      }
      else if (super::_conf.queue_limit==0)
      {
        callback_null_( std::move(callback) );
        ++_drop_count_limit;
        DAEMON_LOG_WARNING("wfc::provider потеряный запрос. Превышен queue_limit=" << super::_conf.queue_limit 
                         << ". Всего потеряно drop_count=" << this->drop_count_()
                         << PROVIDER_SHOW_COUNTERS(this)
        ) //
        return;
      }
    }
    

    // Далее нужна обертка
    post_function f = this->wrap_<N>(mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)...); 
    
    if ( _clicall.size() < super::_conf.max_waiting  )
    {
      auto result = f();
      if ( this->update_waiting_(result, f) )
        return;
    }
    
    if ( _queue.size() < super::_conf.queue_limit )
    {
      _queue.push_back( f );
      if ( _queue.size() > super::_conf.queue_warning )
      {
        DAEMON_LOG_WARNING("wfc::provider: Превышен queue_warning=" << super::_conf.queue_warning 
                         << ". Всего потеряно drop_count=" << this->drop_count_()
                         << PROVIDER_SHOW_COUNTERS(this))
      }
    }
    else
    {
      callback_null_(std::move(callback) );
      ++_drop_count_limit;
      DAEMON_LOG_WARNING("wfc::provider потеряный запрос. Превышен queue_limit=" << super::_conf.queue_limit 
                         << ". Всего потеряно drop_count=" << this->drop_count_()
                         << PROVIDER_SHOW_COUNTERS(this) )
    }
  }

private:
  
  template<int N, typename Req, typename OrigCallback, typename Callback, typename... Args>
  static clicall_pair send_( 
    std::shared_ptr<self> pthis,
    void (interface_type::*mem_ptr)(Req, OrigCallback, Args... args), 
    std::shared_ptr<Req> req, 
    Callback callback, 
    Args... args
  )
  {
    size_t call_id = ++pthis->_call_id_counter;
    
    clicall_pair result = {pthis->null_id, call_id};
    if ( auto cli = pthis->get_( result.first ) )
    {
      OrigCallback wcallback = pthis->wrap_callback_<OrigCallback>( fas::int_<N>(), result.first, call_id, callback);
      
      std::shared_ptr<deadline_timer> timer;
      
      if (pthis->_conf.max_waiting != 0 )
      {
        pthis->_clicall.insert({result.first, call_id});
        auto call_itr = pthis->_callclipost.insert({call_id, clipost{result.first, nullptr, nullptr}}).first;
        
        if (pthis->_conf.wait_timeout_ms != 0 )
        {
          timer = std::make_shared<deadline_timer>(pthis->_io_service);
          std::get<2>(call_itr->second) = timer;
        }
      }
      
      try
      {
        if (pthis->_conf.max_waiting != 0 )
        {
          // Копируем запрос, т.к. возможен повторный вызов
          auto req_for_send = std::make_unique<typename Req::element_type>(**req);
          if (timer)
          {
            timer->expires_from_now(boost::posix_time::milliseconds(pthis->_conf.wait_timeout_ms));
            timer->async_wait( std::bind(
                &provider<Itf>::mt_deadline_<Callback>, 
                pthis, 
                result.first, 
                call_id,
                callback // <- не обернутый callback
            ) );
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
    }
    else
    {
      DAEMON_LOG_WARNING("provider::send_: соединение потеряно")
    }
    return result;
  }
  
  template<typename Resp, typename... Args>
  static void mt_confirm_( 
    std::shared_ptr<self> pthis,
    size_t call_id,
    std::function<void(Resp, Args...)> callback,
    std::shared_ptr<Resp> resp, 
    Args... args
  )
  {
    {
      std::lock_guard<mutex_type> lk(pthis->_mutex); 
      auto call_itr = pthis->_callclipost.find(call_id);
      if ( call_itr == pthis->_callclipost.end() )
      {
        ++pthis->_orphan_count;
        return;
      }
      
      ++pthis->_confirm_count;
      if ( std::get<2>(call_itr->second) )
      {
        std::get<2>(call_itr->second)->cancel();
        std::get<2>(call_itr->second)=nullptr;
      }
      pthis->_callclipost.erase(call_itr);
      pthis->_clicall.erase({std::get<0>(call_itr->second),call_id});
      pthis->process_queue_();
    }
      
    if ( callback!=nullptr )
      callback( std::move(*resp), std::forward<Args>(args)... );
  }


  template<typename Callback>
  static void mt_deadline_( 
    std::shared_ptr<self> pthis,
    size_t client_id, 
    size_t call_id,
    Callback callback
  )
  {
    std::lock_guard<mutex_type> lk( pthis->_mutex );
    
    auto call_itr = pthis->_callclipost.find(call_id);
    if ( call_itr == pthis->_callclipost.end() )
      return;
    
    auto cli_itr = pthis->_clicall.find( { std::get<0>(call_itr->second), call_itr->first});
    if ( cli_itr == pthis->_clicall.end() )
    {
      DAEMON_LOG_FATAL("provider::process_queue_: ошибка логики")
      abort();
    }

    if ( client_id != cli_itr->first )
    {
      DAEMON_LOG_FATAL("provider::process_queue_: ошибка логики")
      abort();
      return;
    }
    
    ++pthis->_drop_count_wait;
    DAEMON_LOG_WARNING("wfc::provider потеряный запрос. Превышен wait_timeout_ms=" << pthis->_conf.wait_timeout_ms
                        << ". Всего потеряно drop_count=" << pthis->drop_count_()
                        << PROVIDER_SHOW_COUNTERS(pthis) )

    pthis->callback_null_( std::move(callback) );
    pthis->_callclipost.erase(call_itr);
    pthis->_clicall.erase(cli_itr);
    pthis->process_queue_();
  }
  
  template<int N, typename Req, typename OrigCallback, typename Callback, typename... Args>
  post_function wrap_( 
    void (interface_type::*mem_ptr)(Req, OrigCallback, Args... args), 
    Req req, 
    Callback callback, 
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
  
  /*
  template<typename Resp, typename ...Args>
  std::function<void(Resp, Args...)>
  callback_detect_1(std::function<void(Resp, Args...)>);

  template<typename Resp, typename ...Args>
  std::function<void(Resp, Args...)>
  callback_detect_2(std::function<void(Resp, size_t, Args...)>);
  
  

  template<typename Callback>
  decltype(callback_detect_1(Callback()))
  wrap_callback_(
    fas::int_<1>, 
    size_t client_id, 
    size_t call_id, 
    Callback callback
  ) 
  {
    return this->wrap_callback_1(client_id, call_id, callback);
  }

  template<typename Callback>
  decltype(callback_detect_2(Callback()))
  wrap_callback_(
    fas::int_<2>, 
    size_t client_id, 
    size_t call_id, 
    Callback callback
  )
  {
    return this->wrap_callback_2(client_id, call_id, callback);
  }*/
  
  
  template<typename OrigCallback, typename Resp, typename ...Args>
  //std::function<void(Resp, Args...)>
  OrigCallback
  wrap_callback_(
    fas::int_<1>, 
    size_t /*client_id*/, 
    size_t call_id, 
    std::function<void(Resp, Args...)> callback
  ) 
  {
    if ( super::_conf.max_waiting == 0 )
      return std::move(callback);
    auto pthis = this->shared_from_this();
    auto callback_fun = &provider<Itf>::mt_confirm_<Resp, Args...>;
    return [callback_fun, pthis, call_id, callback](Resp resp, Args... args)
    {
      auto presp = std::make_shared<Resp>(std::move(resp) );
      pthis->_io_service.post( std::bind(
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
  //std::function<void(Resp, Args...)>
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
      pthis->_io_service.post( std::bind(
        callback_fun,
        pthis,
        call_id,
        callback,
        presp,
        client_id, std::forward<Args>(args)...
      ) );
    };
  }

  bool update_waiting_(const clicall_pair& result, const post_function& f)
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
  
  int process_queue_()
  {
    int count = 0;
    while ( !_queue.empty() )
    {
      
      if ( super::_conf.max_waiting!=0 && _callclipost.size() >= super::_conf.max_waiting )
        break;

      auto f = _queue.front();
      if ( f==nullptr )
      {
        DAEMON_LOG_FATAL("provider::process_queue_: ошибка логики")
        abort();
      }
      
      auto result = f();
      
      if ( !this->update_waiting_(result, f) )
        break;
      _queue.pop_front();
    }
    return count;
  }

private:
  
  ::wfc::io_service& _io_service;
  ::wfc::io_service::work _io_work;
  size_t _call_id_counter;
  callclipost_map _callclipost;
  clicall_set _clicall;
  delayed_queue _queue;
  
  size_t _post_count;
  size_t _confirm_count;
  // Повторные вызовы
  size_t _recall_count;
  // Потерянные вызовы (ответы пришедшие после recall или когда отвалились по timeout)
  size_t _orphan_count;
  // сброшенные по timeout
  size_t _drop_count_wait;
  size_t _drop_count_limit;
  
};

}}
