#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <memory>

namespace wfc{ namespace gateway{ 

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


  sequenced_provider(const provider_config& conf)
    : super(conf)
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
    f();
    //Callback wrapped = this->wrap_callback_( callback );
    // 1. Нет коннекта - в очередь
    // 2. Есть коннект
    // 2.1 Пустая очередь - отправляем
    /*
    if ( auto cli = super::get() )
    {
      (cli.get()->*mem_ptr)( std::move(req), std::move(callback), std::forward<Args>(args)... );
    }
    */
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
    std::lock_guard<mutex_type> lk( pthis->mutex() );
    if ( auto cli = pthis->get_() )
    {
      (cli.get()->*mem_ptr)( std::move(*req), std::move(callback), std::forward<Args>(args)... );
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
    return [pthis, callback](Resp req, Args... args)
    {
      if ( callback!=nullptr)
      {
        callback( std::move(req), std::forward<Args>(args)... );
      }
    };
  }

};

}}
