#pragma once

#include <wfc/gateway/provider/iprovider.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <memory>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class provider_base
  : public std::enable_shared_from_this< provider_base<Itf> >
{
public:
  
  typedef Itf interface_type;
  typedef std::shared_ptr<interface_type> interface_ptr;
  
  typedef std::function<void(interface_ptr)> send_func;
  
  provider_base(const provider_config& conf)
    : _conf(conf)
  {
  }
  
  void startup(size_t /*clinet_id*/, interface_ptr /*ptr*/ ) 
  {
    
  }

  void shutdown(size_t /*clinet_id*/) 
  {
    
  }
  
  template<typename Req, typename Resp>
  void post( 
    void (interface_type::* mem_ptr)(Req, std::function<void(Resp)>), 
    Req req, 
    std::function<void(Resp)> callback
  )
  {
    if ( !_conf.enabled )
      return;
      
    if ( auto cli = this->_ready_for_send() )
    {
      cli.get()->*mem_ptr( std::move(req),  this->_wrap_callback(std::move(callback)));
    }

    auto fn = this->_wrap( mem_ptr, std::move(req), callback);
    
    this->send( fn );
  }

  
  virtual void confirm() = 0;
  
  virtual void rollback() = 0;
  
  virtual void send(send_func fn) = 0;
  
private:
  
  bool _ready() const
  {
    return true;
  }

  interface_ptr _ready_for_send() const
  {
    // if _ready()
    //    1. sequnce_mode: if очередь пуста
    //    2. !sequnce_mode: true
    return interface_ptr();
  }
  
  interface_ptr _capture_for_send() 
  {
    if ( this->_ready_for_send )
    {
      // if (sequnce_mode)
      //    { wait = true}
      //
    }
    return nullptr;
  }

  template<
    typename Req, 
    typename Resp,
    typename... Args
  >
  send_func
  _wrap( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(Resp)> callback, 
    Args... args
  )
  {
    /*
// ----------------------------------------------------------------------
    auto preq = std::make_shared<Req>( std::move(req) );
    auto pthis = this->shared_from_this();
    // обход бага завата Args...
    return std::bind(
      [pthis](
        std::unique_lock<mutex_type>& lk, 
        void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
        std::shared_ptr<Req> preq, 
        std::function<void(Resp)> callback, 
        Args... args
      )
      {
        if ( auto cli = pthis->self_get() )
        {
          pthis->_wait_flag = true;
          lk.unlock();
          (cli.get()->*mem_ptr)( std::move(*preq), callback, args... );
          lk.lock();
          return true;
        }
        else
        {
          pthis->_wait_flag = false;
          return false;
        }

      }, 
      std::placeholders::_1,
      mem_ptr, 
      preq, 
      this->wrap_callback( callback ), 
      std::forward<Args>(args)...
    );
    */
  }

  

private:
  provider_config _conf;
};

}}
