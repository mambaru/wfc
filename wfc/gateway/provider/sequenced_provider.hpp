#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <wfc/memory.hpp>
#include <wfc/logger.hpp>
#include <wfc/io_service.hpp>
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

  sequenced_provider( ::wfc::io_service& io, const provider_config& conf)
    : super(conf)
    , _io_service(io)
    , _time_point( std::chrono::milliseconds(0) )
    , _wait_client_id(null_id)
    , _wait_call_id(0)
    , _call_id_counter(0)
    , _recall_count(0)
    , _orphan_count(0)
    , _drop_count(0)
    , _delayed_pop(false)
    , _lock_counter(0)
  {
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
    return _drop_count;
  }

  size_t queue_size() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _queue.size();
  }

  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );

    super::startup_(client_id, ptr);
    if ( _wait_client_id == null_id )
    {
      if ( this->process_queue_() )
      {
        // не правильно считает
        // ++_recall_count;
      }
    }
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
    {
      return;
    }
      
    
    if ( _queue.size() != 1 )
    {
      // Проверить наличие коннектов. Если есть то fail
      return;
    }
    
    f();
  }

private:
  
  bool _recursive_flag = false;
  
  
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
      DAEMON_LOG_FATAL("" << "Logic error " << pthis->_wait_client_id << " thread id=" << *(reinterpret_cast<size_t*>(pthis.get())))
      abort();
    }
     
    bool ready = false;
    // вызов только из критической секции
    pthis->_wait_client_id = null_id;
    if ( auto cli = pthis->get_( pthis->_wait_client_id ) )
    {
      pthis->_wait_call_id = call_id;
      pthis->_mutex.unlock();
      
      try
      {
        // Копируем запрос, т.к. возможен повторный вызов
        auto req_for_send = std::make_unique<typename Req::element_type>(**req);
        (cli.get()->*mem_ptr)( std::move(req_for_send), std::move(callback), std::forward<Args>(args)... );
      } 
      catch ( ... )
      {
        DAEMON_LOG_FATAL("2 sequenced_provider::send_ unhandled exception")
        abort();
      }
      pthis->_mutex.lock();
      ready = true;
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
  
  void result_ready_()
  {
    if ( super::_conf.timeout_ms!=0 )
      _time_point = clock_t::now();
    
    this->pop_();
    this->process_queue_();
  }
  
  template<typename Resp, typename... Args>
  static void callback_( 
    std::shared_ptr<self> /*pthis*/,
    size_t /*call_id*/,
    std::function<void(Resp, Args...)> /*callback*/,
    std::shared_ptr<Resp> /*resp*/, 
    Args... /*args*/
  )
  {
    
  }

  
  template<typename Resp, typename ...Args>
  std::function<void(Resp, Args...)>
  wrap_callback_(std::function<void(Resp, Args...)> callback) 
  {
    typedef std::function<void(Resp, Args...)> callback_type;
    //static std::atomic<int> x(0);
    
    auto call_id = this->_call_id_counter;
    auto pthis = this->shared_from_this();
    
    /*callback_type result = std::bind(
      &sequenced_provider<Itf>::callback_<Resp, Args...>,
      pthis,
      call_id,
      callback
    );
    
    return result;*/

    return [pthis, call_id, callback](Resp resp, Args... args)
    {
      {
        std::lock_guard<mutex_type> lk(pthis->_mutex); 

        if ( pthis->_wait_call_id != call_id )
          return;
        
        if ( pthis->_recursive_flag )
        {
          pthis->_recursive_flag = false;
        }
        else
        {
          pthis->result_ready_();
        }
      }
      
      if ( callback!=nullptr )
        callback( std::move(resp), std::forward<Args>(args)... );

      return;
    };
  }
  
  void pop_()
  {
    _queue.pop();
    _wait_client_id = null_id;
    _wait_call_id = 0;
    //_time_point = std::chrono::milliseconds(0);
  }

  int process_queue_()
  {
    int result = 0;
    while ( !_queue.empty() )
    {
      _recursive_flag = true;
      _queue.front()();
      ++result;
      if ( !_recursive_flag )
      {
        this->pop_();
      }
      else
      {
        _recursive_flag = false;
      }
    }
    return result;
  }

  void check_timeout_()
  {
    if ( super::_conf.timeout_ms!=0 && !_queue.empty())
    {
      time_point_t now = clock_t::now();
    
      if ( super::_conf.timeout_ms <= std::chrono::duration_cast < std::chrono::milliseconds>( now - _time_point ).count() )
      {
        ++_drop_count;
        this->pop_();
        DAEMON_LOG_WARNING("Сброс ожидания запроса по timeout");
      }
    }
  }

private:
  ::wfc::io_service& _io_service;
  time_point_t _time_point;
  size_t _wait_client_id;
  size_t _wait_call_id;
  size_t _call_id_counter;
  delayed_queue _queue;
  
  // Повторные вызовы
  size_t _recall_count;
  // Потерянные вызовы
  size_t _orphan_count;
  // сброшенные по timeout
  size_t _drop_count;

  bool _delayed_pop;
  std::atomic<size_t> _lock_counter;
};

}}
