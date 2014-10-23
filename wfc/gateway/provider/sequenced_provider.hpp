#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <wfc/memory.hpp>
#include <wfc/logger.hpp>
#include <wfc/io_service.hpp>

#include <boost/asio.hpp>
#include <memory>
#include <queue>

namespace wfc{ namespace gateway{ 

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
    , _io_work(io)
    , _deadline_timer(io)
    , _wait_client_id(null_id)
    , _wait_call_id(0)
    , _call_id_counter(0)
    , _recall_count(0)
    , _orphan_count(0)
    , _drop_count(0)
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
      _recall_count += this->process_queue_();
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
      _recall_count += this->process_queue_();
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
  
  static void deadline_( 
    std::shared_ptr<self> pthis,
    size_t client_id, 
    size_t call_id)
  {
    std::lock_guard<mutex_type> lk( pthis->_mutex );
    
    
    if (client_id != pthis->_wait_client_id 
        || call_id != pthis->_wait_call_id)
    {
      // TODO: log
      return;
    }
    
    ++pthis->_drop_count;
    // TODO: log
    pthis->result_ready_();
  }
    

  
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
      size_t client_id = pthis->_wait_client_id;
      pthis->_wait_call_id = call_id;
      pthis->_mutex.unlock();
      
      try
      {
        // Копируем запрос, т.к. возможен повторный вызов
        auto req_for_send = std::make_unique<typename Req::element_type>(**req);
        if (pthis->_conf.timeout_ms != 0 )
        {
          pthis->_deadline_timer.expires_from_now(boost::posix_time::milliseconds(pthis->_conf.timeout_ms));
          pthis->_deadline_timer.async_wait( std::bind(
              &sequenced_provider<Itf>::deadline_, 
              pthis, 
              client_id, 
              call_id
          ) );
        }
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
    this->pop_();
    this->process_queue_();
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

      if ( pthis->_wait_call_id != call_id )
      {
        ++pthis->_orphan_count;
        return;
      }
      pthis->result_ready_();
    }
      
    if ( callback!=nullptr )
      callback( std::move(*resp), std::forward<Args>(args)... );
  }

  
  template<typename Resp, typename ...Args>
  std::function<void(Resp, Args...)>
  wrap_callback_(std::function<void(Resp, Args...)> callback) 
  {
    auto call_id = this->_call_id_counter;
    auto pthis = this->shared_from_this();

    auto callback_fun = &sequenced_provider<Itf>::mt_confirm_<Resp, Args...>;
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
  
  void pop_()
  {
    _queue.pop();
    _wait_client_id = null_id;
    _wait_call_id = 0;
  }

  int process_queue_()
  {
    int result = 0;
    while ( !_queue.empty() )
    {
      _queue.front()();
      ++result;
      if ( super::_conf.mode == provider_mode::sequenced )
        break;
    }
    return result;
  }

private:
  ::wfc::io_service& _io_service;
  ::wfc::io_service::work _io_work;
  ::boost::asio::deadline_timer _deadline_timer;
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
};

}}
