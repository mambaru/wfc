#pragma once

#include <map>
#include <mutex>
#include <queue>
#include <list>
#include <wfc/thread/rwlock.hpp>
#include <wfc/thread/spinlock.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace gateway{ 
  
template<typename Itf>
class provider
  // : public Itf
{
  typedef ::wfc::spinlock basic_mutex_type;
  typedef ::wfc::rwlock<basic_mutex_type> mutex_type;
  typedef Itf interface_type;
  typedef provider<interface_type> self;
  typedef std::weak_ptr<interface_type> interface_ptr;
  typedef std::map<size_t, interface_ptr> clinet_map;
  typedef std::queue< std::function<void()> > delayed_queue;
  
public:
  
  typedef std::function<void(size_t)> shudown_handler;
  typedef std::list<shudown_handler> shudown_handler_list;
  
  provider()
    : _cli_itr(_clients.end())
  {}
  
  // Когда вызывать, определяеться в method_list
  void startup(size_t clinet_id, interface_ptr ptr )
  {
    delayed_queue dq;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      this->_clients[clinet_id] = ptr;
      this->_cli_itr = this->_clients.begin();
      dq.swap(_delayed_queue);
    }
    
    while ( !dq.empty() )
    {
      dq.front()();
      dq.pop();
    }
  }
    
  // Когда вызывать, определяеться в method_list
  void shutdown(size_t clinet_id )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->_clients.erase(clinet_id);
    this->_cli_itr = this->_clients.begin();
    
    for ( auto& sh: _shudown_handlers )
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

  interface_ptr get(size_t& client_id)
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    
    if ( _clients.empty() )
      return interface_ptr();
    if ( _cli_itr == _clients.end() )
      _cli_itr = _clients.begin();
    client_id = _cli_itr->first;
    return (_cli_itr++)->second;
  }

  interface_ptr get()
  {
    size_t client_id = 0;
    return this->get(client_id);
  }

  template<
    typename Req, 
    typename Resp,
    typename... Args
  >
  std::function<void()> wrap( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(Resp)> callback, 
    Args... args
  )
  {
    auto preq = std::make_shared<Req>( std::move(req) );
    // обход бага завата Args...
    return std::bind(
      [this](
        void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
        std::shared_ptr<Req> preq, 
        std::function<void(Resp)> callback, 
        Args... args
      )
      {
        this->post(mem_ptr, std::move(*preq), callback, args...);
      }, 
      mem_ptr, 
      preq, 
      callback, 
      std::forward<Args>(args)...
    );
  }

  template<
    typename Req, 
    typename Resp,
    typename... Args
  >
  std::function<void()> wrap2( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(size_t, Resp)> callback, 
    Args... args
  )
  {
    auto preq = std::make_shared<Req>( std::move(req) );
    // обход бага завата Args...
    return std::bind(
      [this](
        void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
        std::shared_ptr<Req> preq, 
        std::function<void(size_t, Resp)> callback, 
        Args... args
      )
      {
        this->post2(mem_ptr, std::move(*preq), callback, args...);
      }, 
      mem_ptr, 
      preq, 
      callback, 
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
    if (auto cli = this->get().lock() )
    {
      (cli.get()->*mem_ptr)( std::move(req), callback, args... );
    }
    else
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _delayed_queue.push( this->wrap( mem_ptr, std::move(req), callback, std::forward<Args>(args)...) );
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
    size_t client_id = 0;
    if (auto cli = this->get(client_id).lock() )
    {
      (cli.get()->*mem_ptr)( 
        std::move(req), 
        [client_id, callback](Resp resp)
        {
          callback( client_id, std::move(resp));
        }, 
        args... 
      );
    }
    else
    {
      std::lock_guard<mutex_type> lk(_mutex);
      auto wrp = this->wrap2( mem_ptr, std::move(req), callback, std::forward<Args>(args)...);
      _delayed_queue.push( wrp );
    }
  }
  
  void operator+= ( shudown_handler sh )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _shudown_handlers.push_back(sh);
  }
  
private:
  mutex_type _mutex;
  clinet_map _clients;
  typename clinet_map::iterator _cli_itr;
  delayed_queue _delayed_queue;
  shudown_handler_list _shudown_handlers;
};
  
}}

