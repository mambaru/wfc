#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <wfc/logger.hpp>
#include <memory>
#include <queue>

namespace wfc{ namespace gateway{ 

/*
 * Не важно сколько активных клиентов, следующий после ответа 
 */




template<typename Itf>  
class sequenced_provider
  : public basic_provider<Itf, sequenced_provider, std::recursive_mutex>
{
  typedef sequenced_provider<Itf> self;
  typedef basic_provider<Itf, sequenced_provider, std::recursive_mutex> super;
  typedef std::chrono::high_resolution_clock clock_t;
  typedef clock_t::time_point time_point_t;
  
  static const size_t null_id = static_cast<size_t>(-1);

public:
  
  
  typedef typename super::interface_type interface_type;
  typedef typename super::mutex_type mutex_type;
  typedef std::function<bool()> post_function;
  typedef std::queue< post_function > delayed_queue;

  sequenced_provider(const provider_config& conf)
    : super(conf)
    , _time_point( std::chrono::milliseconds(0) )
    , _wait_client_id(null_id)
    , _wait_call_id(0)
    , _call_id_counter(0)
    , _recall_count(0)
    , _lost_callback(0)
    , _timeout_dropped(0)
    , _lock_counter(0)
  {
  }
  
  size_t lost_callback() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _lost_callback;
  }

  size_t recall_count() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _recall_count;
  }

  size_t timeout_dropped() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _timeout_dropped;
  }

  size_t queue_size() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _queue.size();
  }


  virtual void shutdown(size_t client_id)
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    
    super::shutdown_(client_id);
    
    if ( _wait_client_id == client_id )
    {
      DAEMON_LOG_WARNING( 
        "provider(mode=sequenced): закрытие соединения при ожидании подверждения удаленного вызова. "
        "Информация о выполнении вызова отсутствует. ")
      _wait_client_id = null_id;
      _wait_call_id = 0;
      if ( this->process_queue_() )
      {
        ++_recall_count;
      };
    }
  }
  
  bool check() const
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    if ( !_queue.empty() && super::ready_count_() )
    {
      if ( _wait_client_id == null_id || _wait_call_id == 0)
      {
        std::cout << "_wait_client_id: " << _wait_client_id << std::endl;
        std::cout << "_wait_call_id: " << _wait_call_id << std::endl;
        abort();
        return false;
      }
    }
    else 
    {
      if ( _wait_client_id != null_id || _wait_call_id != 0)
      {
        abort();
        return false;
      }
    }
    return true;
  }

  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    super::startup_(client_id, std::move(ptr) );
    if ( _wait_client_id == null_id )
    {
      if ( this->process_queue_() )
      {
        ++_recall_count;
      }
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
    std::lock_guard<mutex_type> lk( super::_mutex );
    
    if ( !super::_conf.enabled )
      return;

    this->check_timeout_();
    
    post_function f = this->wrap_(mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)...); 
    
    _queue.push( f );
    
    if ( _wait_client_id != null_id )
      return;
    
    if ( _queue.size() != 1 )
    {
      // Проверить наличие коннектов. Если есть то fail
      return;
    }
    
    f();
  }

private:
  
  
  template<typename Req, typename Callback, typename... Args>
  static bool send_( 
    std::shared_ptr<self> pthis,
    size_t call_id,
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    std::shared_ptr<Req> req, 
    Callback callback, 
    Args... args
  )
  {
    if ( pthis->_wait_client_id != null_id )
    {
      DAEMON_LOG_FATAL("" << "Logic error " << pthis->_wait_client_id)
      abort();
    }
     
    bool ready = false;
    // вызов только из критической секции
    pthis->_wait_client_id = null_id;
    if ( auto cli = pthis->get_( pthis->_wait_client_id ) )
    {
      try
      {
        pthis->_wait_call_id = call_id;
        pthis->_mutex.unlock();
        (cli.get()->*mem_ptr)( std::move(*req), std::move(callback), std::forward<Args>(args)... );
        pthis->_mutex.lock();
        ready = true;
      } 
      catch ( ... )
      {
        DAEMON_LOG_FATAL("sequenced_provider::send_ unhandled exception")
        abort();
      }
    }
    else
    {
      pthis->_wait_client_id = null_id;
    }
    return ready;
  }
    
  template<typename Req, typename Callback, typename... Args>
  post_function wrap_( 
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    auto preq = std::make_shared<Req>( std::move(req) );
    auto pthis = super::shared_from_this();
    ++this->_call_id_counter;
    
    return std::bind(
      &sequenced_provider<Itf>::send_<Req, Callback, Args...>,
      pthis,
      this->_call_id_counter,
      mem_ptr, 
      preq, 
      this->wrap_callback_(callback), 
      std::forward<Args>(args)...
    );
  }
  
  template<typename Resp, typename ...Args>
  std::function<void(Resp, Args...)>
  wrap_callback_(std::function<void(Resp, Args...)> callback) 
  {
    auto call_id = this->_call_id_counter;
    auto pthis = this->shared_from_this();
    return [pthis, call_id, callback](Resp resp, Args... args)
    {
      if ( callback!=nullptr)
      {
        callback( std::move(resp), std::forward<Args>(args)... );
      }
      
      ++pthis->_lock_counter;
      
      std::lock_guard<mutex_type> lk(pthis->_mutex); 
     
      if ( pthis->_conf.timeout_ms!=0 )
      {
        time_point_t now = clock_t::now();
      }
      
      if ( pthis->_wait_call_id != call_id )
      {
        --pthis->_lock_counter;
        ++pthis->_lost_callback;
        DAEMON_LOG_WARNING("provider(mode=sequenced): идентификатор ответа не соответствует запросу. "
                           "При переоткрытии объекта во время ожидания ответа был отправлен новый запрос. "
                           "Это ответ на первый запрос - игнорируеться. Запрос на объект был доставлен дважды. ")
       return;
      }

      bool flag = pthis->_wait_client_id != null_id;
      
      pthis->pop_();

      // _lock_counter возможно не срабатывает и очередь повисает
      if ( flag && pthis->_lock_counter == 1 )
      {
        // При синхронных ответах и post из нескольких потоках, возможна бесконечная рекурсия pthis->process_queue_
        // Поэтому если кто-то ожидает мьютекс, то очередь не обрабатываем
        pthis->process_queue_();
      }
      else if ( !flag )
      {
        COMMON_LOG_MESSAGE("provider(mode=sequenced): получение ответа после отключения последнего объекта.")
      }

      --pthis->_lock_counter;
      /*
      
      
      
      if ( pthis->_wait_client_id == null_id )
      {
        --pthis->_lock_counter;
        DAEMON_LOG_WARNING("" << " _wait_client_id сброшен callback: pthis->_wait_client_id == null_id")
       return;
      }
      
      pthis->_queue.pop();
      
      pthis->_wait_client_id = null_id;
      pthis->_wait_call_id = 0;
      if ( pthis->_lock_counter == 1 )
        pthis->process_queue_();
      --pthis->_lock_counter;
      */
      
    };
  }
  
  void pop_()
  {
    _queue.pop();
    _wait_client_id = null_id;
    _wait_call_id = 0;
    //_time_point = std::chrono::milliseconds(0);
  }

  bool process_queue_()
  {
    if ( _queue.empty() )
      return false;

    auto f = _queue.front();
    return f();
  }

  void check_timeout_()
  {
    if ( super::_conf.timeout_ms!=0 && !_queue.empty())
    {
      time_point_t now = clock_t::now();
    
      if ( super::_conf.timeout_ms <= std::chrono::duration_cast < std::chrono::milliseconds>( now - _time_point ).count() )
      {
        ++_timeout_dropped;
        this->pop_();
        DAEMON_LOG_WARNING("Сброс ожидания запроса по timeout");
      }
    }
  }

private:
  time_point_t _time_point;
  size_t _wait_client_id;
  size_t _wait_call_id;
  size_t _call_id_counter;
  delayed_queue _queue;
  
  // Повторные вызовы
  size_t _recall_count;
  // Потерянные вызовы
  size_t _lost_callback;
  // сброшенные по timeout
  size_t _timeout_dropped;
  
  std::atomic<size_t> _lock_counter;
};

}}
