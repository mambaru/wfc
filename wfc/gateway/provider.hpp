#pragma once

#include <map>
#include <mutex>
#include <queue>
#include <list>
#include <wfc/thread/rwlock.hpp>
#include <wfc/thread/spinlock.hpp>
#include <wfc/memory.hpp>
#include <wfc/gateway/provider_config.hpp>

namespace wfc{ namespace gateway{ 
  
template<typename Itf>
class provider
  : public std::enable_shared_from_this< provider<Itf> >
{
  typedef ::wfc::spinlock basic_mutex_type;
  typedef ::wfc::rwlock<basic_mutex_type> mutex_type;
  typedef Itf interface_type;
  typedef provider<interface_type> self;
  typedef std::shared_ptr<interface_type> interface_ptr;
  typedef std::map<size_t, interface_ptr> clinet_map;
  typedef std::queue< std::function<bool(std::unique_lock<mutex_type>&)> > delayed_queue;
  
public:
  
  typedef std::function<void(size_t)> shudown_handler;
  typedef std::list<shudown_handler> shudown_handler_list;
  
  provider(const provider_config& conf)
    : _wait_flag(false)
    , _conf(conf)
    , _client_count(0)
    , _cli_itr(_clients.end())
  {}
  
  provider(const provider& ) = delete;
  
  bool ready() const
  {
    return _client_count!=0;
  }
  
  void reconfigure(const provider_config& conf)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _conf = conf;
    if ( !_conf.enabled )
      while ( !_delayed_queue.empty() ) _delayed_queue.pop();
    
  }
  
  void process_sequence_queue(std::unique_lock<mutex_type>& lk, bool last_call_ready)
  {
    if ( !last_call_ready && _wait_flag )
    {
      return;
    }

    if ( last_call_ready )
    {
      _delayed_queue.pop();
    }
    
    if ( _delayed_queue.empty() )
    {
      return;
    }
    
    auto fn = _delayed_queue.front();
    if ( fn == nullptr )
    {
      abort();
    }
    fn(lk);
  }

  void process_non_sequence_queue(std::unique_lock<mutex_type>& lk)
  {
    while ( !_delayed_queue.empty() )
    {
      auto fn = _delayed_queue.front();
      _delayed_queue.pop();
      fn(lk);
    }
  }

  void process_queue(std::unique_lock<mutex_type>& lk, bool last_call_ready = false)
  {
    if ( !this->ready() )
      return;
    
    //std::unique_lock<mutex_type> lk(_mutex);

    if ( _delayed_queue.empty() )
      return;
    
    if ( _conf.sequence_mode )
    {
      this->process_sequence_queue(lk, last_call_ready);
    }
    else
    {
      this->process_non_sequence_queue(lk);  
    }
  }

  // Когда вызывать, определяеться в method_list
  void startup(size_t clinet_id, std::shared_ptr<interface_type> ptr )
  {
    std::unique_lock<mutex_type> lk(_mutex);
    
    this->_clients[clinet_id] = ptr;
    this->_client_count = this->_clients.size();
    this->_cli_itr = this->_clients.begin();
    this->process_queue(lk, false);
  }
    
  // Когда вызывать, определяеться в method_list
  void shutdown(size_t clinet_id)
  {
    std::unique_lock<mutex_type> lk(_mutex);

    this->_clients.erase(clinet_id);
    this->_cli_itr = this->_clients.begin();
    this->_client_count = this->_clients.size();
    auto shs = _shudown_handlers;
    lk.unlock();

    for ( auto& sh: shs )
      sh( clinet_id );
  }

  interface_ptr find(size_t client_id)
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    
    auto itr = _clients.find(client_id);
    if ( itr == _clients.end() )
      return interface_ptr();
    return itr->second;
  }
  
  interface_ptr self_get(size_t& client_id)
  {
    if ( !_conf.enabled || _clients.empty() )
      return nullptr;
    if ( _cli_itr == _clients.end() )
      _cli_itr = _clients.begin();
    client_id = _cli_itr->first;
    return (_cli_itr++)->second;
  }

  interface_ptr get(size_t& client_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return this->self_get(client_id);
  }

  interface_ptr get()
  {
    size_t client_id = 0;
    return this->get(client_id);
  }

  interface_ptr self_get()
  {
    size_t client_id = 0;
    return this->self_get(client_id);
  }

  
  template<typename Resp>
  std::function<void(Resp)> wrap_callback(std::function<void(Resp)> callback) 
  {
    if (_conf.sequence_mode)
    {
      auto pthis = this->shared_from_this();
      return [pthis, callback](Resp req)
      {
        if ( callback!=nullptr)
        {
          callback( std::move(req) );
        }
        std::unique_lock<mutex_type> lk(pthis->_mutex);
        pthis->_wait_flag = false;
        pthis->process_queue(lk, true);
      };
    }
    else
    {
      return callback;
    }
  }

  template<typename Resp>
  std::function<void(size_t, Resp)> wrap_callback2(std::function<void(size_t, Resp)> callback) 
  {
    if (_conf.sequence_mode)
    {
      auto pthis = this->shared_from_this();
      return [pthis, callback](size_t io_id, Resp req)
      {
        if ( callback!=nullptr)
        {
          callback( io_id, std::move(req) );
        }
        
        std::unique_lock<mutex_type> lk(pthis->_mutex);
        pthis->_wait_flag = false;
        pthis->process_queue(lk, true);
      };
    }
    else
    {
      return callback;
    }
  }

  template<
    typename Req, 
    typename Resp,
    typename... Args
  >
  std::function<bool(std::unique_lock<mutex_type>&)> wrap( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(Resp)> callback, 
    Args... args
  )
  {
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
  }

  template<
    typename Req, 
    typename Resp,
    typename... Args
  >
  std::function<bool(std::unique_lock<mutex_type>&)> wrap2( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(size_t, Resp)> callback, 
    Args... args
  )
  {
    auto preq = std::make_shared<Req>( std::move(req) );
    auto pthis = this->shared_from_this();
    auto wcallback =       [pthis](
        std::unique_lock<mutex_type>& lk,
        void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
        std::shared_ptr<Req> preq, 
        std::function<void(size_t, Resp)> callback, 
        Args... args
      ) -> bool
      {
        size_t client_id = 0;
        if ( auto cli = pthis->self_get(client_id) )
        {
          pthis->_wait_flag = true;
          lk.unlock();
          (cli.get()->*mem_ptr)( 
            std::move(*preq), 
            [client_id, callback](Resp resp)
            {
              callback( client_id, std::move(resp));
            }, 
            args... 
          );
          lk.lock();
          return true;
        }
        else
        {
          pthis->_wait_flag = false;
          return false;
        }
      };

    return std::bind(
      wcallback, 
      std::placeholders::_1,
      mem_ptr, 
      preq, 
      this->wrap_callback2( callback ), 
      std::forward<Args>(args)...
    );
  }

  
  
  template<typename Req, typename Resp, typename... Args>
  void post( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(Resp)> callback, 
    Args... args
  )
  {
    std::unique_lock<mutex_type> lk(_mutex);
    
    auto cli = this->self_get();
    
    if ( !_conf.enabled )
      return;

    auto fn = this->wrap( mem_ptr, std::move(req), callback, std::forward<Args>(args)...);
    
    if ( _conf.sequence_mode )
    {
      _delayed_queue.push( fn );
      this->process_sequence_queue( lk, false);
    }
    else
    {
      if ( cli!=nullptr )
      {
        fn(lk);
      }
      else
      {
        _delayed_queue.push( fn );
      }
    }
  }
  
  /* для переопределенного callback с id клиента*/
  
  template<typename Req, typename Resp, typename... Args>
  void post2( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(size_t, Resp)> callback, 
    Args... args
  )
  {
    std::unique_lock<mutex_type> lk(_mutex);
    
    auto cli = this->self_get();

    if ( !_conf.enabled )
      return;

    auto fn = this->wrap2( mem_ptr, std::move(req), callback, std::forward<Args>(args)...);
    
    if ( _conf.sequence_mode )
    {
      _delayed_queue.push( fn );
      this->process_sequence_queue( lk, false);
    }
    else
    {
      if (cli!=nullptr )
      {
        fn(lk);
      }
      else
      {
        _delayed_queue.push( fn );
      }
    }
  }
  
  void operator+= ( shudown_handler sh )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _shudown_handlers.push_back(sh);
  }
  
  size_t queue_size() const
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return _clients.size();
  }
  
private:
  
  // Ожидание ответа, на вызов из головы очереди ожидания (только в sequence_mode)
  std::atomic<bool> _wait_flag;
  // Очередь ожидания (только для sequence_mode)
  delayed_queue _delayed_queue;
  
  provider_config _conf;
  //std::atomic<bool> _sequence_mode;
  std::atomic<size_t>  _client_count;
  mutable mutex_type _mutex;
  clinet_map _clients;
  typename clinet_map::iterator _cli_itr;
  shudown_handler_list _shudown_handlers;
};
  
}}

