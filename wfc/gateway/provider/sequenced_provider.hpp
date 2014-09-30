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
  
  static const size_t null_id = static_cast<size_t>(-1);
public:
  
  typedef typename super::interface_type interface_type;
  typedef typename super::mutex_type mutex_type;
  typedef std::function<void()> post_function;
  typedef std::queue< post_function > delayed_queue;

  //int warning_count = 0;
  sequenced_provider(const provider_config& conf)
    : super(conf)
    , _wait_client_id(null_id)
    , _wait_call_id(0)
    , _call_id_counter(0)
    , _lost_call(0)
    , _lost_callback(0)
  {
  }
  
  size_t lost_call() const 
  {
    std::lock_guard<mutex_type> lk( super::_mutex ); //!
    return _lost_call;
  }
  
  size_t lost_callback() const 
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    return _lost_callback;
  }

  virtual void shutdown(size_t client_id)
  {
    std::lock_guard<mutex_type> lk( super::_mutex ); //!
    
    //std::cout << "shutdown { " << client_id << std::endl;
    
    super::shutdown_(client_id);
    
    if ( _wait_client_id == client_id )
    {
      ++_lost_call;
      //DAEMON_LOG_WARNING(":" /*<< ++warning_count*/ << " provider(mode=sequenced): закрытие соединения при ожидании подверждения удаленного вызова. Информация об успешном/неуспешном вызове отсутствует.")
      _wait_client_id = null_id;
      _wait_call_id = 0;
      this->process_queue_();
    }
  }

  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    // std::cout << "startup { " << client_id << std::endl;
    super::startup_(client_id, std::move(ptr) );
    if ( _wait_client_id == null_id )
      this->process_queue_();
    // std::cout << "} startup " << client_id << std::endl;
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
    
    post_function f = this->wrap_(mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)...); 
    
    _queue.push( f );
    
    if ( _wait_client_id != null_id )
      return;
    
    /*
    size_t size = _queue.size();
    if ( size != 1 )
    {
      DAEMON_LOG_FATAL("Fail queue size: " << size)
      abort();
    }
    */
    
    if ( _queue.size() != 1 )
    {
      // Проверить наличие коннектов. Если есть то fail
      return;
    }
    
    f();
  }

private:
  
  
  template<typename Req, typename Callback, typename... Args>
  static void send_( 
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
     
    // вызов только из кретической секции
    pthis->_wait_client_id = null_id;
    if ( auto cli = pthis->get_( pthis->_wait_client_id ) )
    {
      try
      {
        pthis->_wait_call_id = call_id;
        // std::cout << "send_ ready!!!" << pthis->_wait_client_id << std::endl;
        pthis->_mutex.unlock();
        (cli.get()->*mem_ptr)( std::move(*req), std::move(callback), std::forward<Args>(args)... );
        pthis->_mutex.lock();
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
      std::cout << "send_ NOT ready!!! " << pthis->_call_id_counter << std::endl;
    }
  
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
        callback( std::move(resp), std::forward<Args>(args)... );

      std::lock_guard<mutex_type> lk(pthis->_mutex); //!
      
      
      if ( pthis->_wait_call_id != call_id )
      {
        ++pthis->_lost_callback;
        DAEMON_LOG_WARNING("" << " екправильный call_id " << pthis->_wait_call_id << " " << call_id)
        //abort();
       return;
      }

      pthis->_queue.pop();
      
      if ( pthis->_wait_client_id == null_id )
      {
        DAEMON_LOG_WARNING("" << " _wait_client_id сброшен callback: pthis->_wait_client_id == null_id")
       return;
        //abort();
      }
      
      pthis->_wait_client_id = null_id;
      pthis->_wait_call_id = 0;
      pthis->process_queue_();
      
    };
  }

  void process_queue_()
  {
    if ( _queue.empty() )
      return;
    auto f = _queue.front();
    f();
    
    //std::cout << "process_queue_ { size=" << _queue.size() << std::endl;
    //auto f = _queue.front();
    //this->_mutex.unlock();
    //f();
    //this->_mutex.lock();
    //std::cout << "} process_queue_" << std::endl;
  }
  
private:
  size_t _wait_client_id;
  size_t _wait_call_id;
  size_t _call_id_counter;
  delayed_queue _queue;
  
  size_t _lost_call;
  size_t _lost_callback;
};

}}
