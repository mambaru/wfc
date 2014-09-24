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
  : public basic_provider<Itf, sequenced_provider>
{
  typedef sequenced_provider<Itf> self;
  typedef basic_provider<Itf, sequenced_provider> super;
  
public:
  
  typedef typename super::interface_type interface_type;
  typedef typename super::mutex_type mutex_type;
  typedef std::function<void()> post_function;
  typedef std::queue< post_function > delayed_queue;
  //typedef std::map<size_t, post_function> wait_map;


  sequenced_provider(const provider_config& conf)
    : super(conf)
    , _wait_client_id(0)
  {
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
    size_t clent_id = 0;
    bool send_flag = false;
    if ( auto cli = this->get_(clent_id) )
    {
      if ( _wait_post == nullptr )
      {
        send_flag = true;
        _wait_post = f;
        _wait_client_id = clent_id;
        f();
      }
    }
    if (!send_flag)
    {
      _queue.push( std::move(f) );
    }
  }

private:
  
  mutex_type& mutex() const
  {
    return super::_mutex;
  }

  
  template<typename Req, typename Callback, typename... Args>
  static void send_( 
    std::shared_ptr<self> pthis,
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    std::shared_ptr<Req> req, 
    Callback callback, 
    Args... args
  )
  {
    // вызов только из кретической секции
    if ( auto cli = pthis->get_( pthis->_wait_client_id ) )
    {
      pthis->mutex().unlock();
      // А если shutdown? 
      try
      {
        (cli.get()->*mem_ptr)( std::move(*req), std::move(callback), std::forward<Args>(args)... );
      } 
      catch ( ... )
      {
        DAEMON_LOG_FATAL("sequenced_provider::send_ unhandled exception")
        abort();
      }
      pthis->mutex().lock();
      return true;
    }
    return false;
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
    
    return std::bind(
      &sequenced_provider<Itf>::send_<Req, Callback, Args...>,
      pthis,
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
    auto pthis = this->shared_from_this();
    return [pthis, callback](Resp resp, Args... args)
    {
      if ( callback!=nullptr)
      {
        callback( std::move(resp), std::forward<Args>(args)... );
      }
      pthis->process_queue();
    };
  }

  void process_queue()
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    _wait_client_id = 0;
    _wait_post = nullptr;
    if ( _queue.empty() )
      return;
    _wait_post = _queue.front();
    _queue.pop();
    _wait_post();
  }
  
private:
  size_t _wait_client_id;
  post_function _wait_post;
  delayed_queue _queue;
  
};

}}
