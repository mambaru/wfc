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
{
  typedef ::wfc::spinlock basic_mutex_type;
  typedef ::wfc::rwlock<basic_mutex_type> mutex_type;
  typedef Itf interface_type;
  typedef provider<interface_type> self;
  typedef std::weak_ptr<interface_type> interface_ptr;
  typedef std::map<size_t, interface_ptr> clinet_map;
  typedef std::queue< std::function<bool()> > delayed_queue;
  
public:
  
  typedef std::function<void(size_t)> shudown_handler;
  typedef std::list<shudown_handler> shudown_handler_list;
  
  provider(const provider_config& conf)
    : _wait_flag(false)
    , _conf(conf)
    , _client_count(0)
    , _cli_itr(_clients.end())
  {}
  
  bool ready() const
  {
    return _client_count!=0;
  }
  
  /*
  void sequence_mode(bool value) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _sequence_mode = value;
  }

  void sequence_mode() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _sequence_mode;
  }
  */
  
  void reconfigure(const provider_config& conf)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _conf = conf;
    if ( !_conf.enabled )
      while ( !_delayed_queue.empty() ) _delayed_queue.pop();
    
  }
  
  void process_sequence_queue(std::unique_lock<mutex_type>& lk)
  {
    auto fn = _delayed_queue.front();
    //_delayed_queue.front() = nullptr;
    lk.unlock();
    bool ok = false;
    while ( this->ready() && !ok )
      ok = fn();
    lk.lock();
    if ( ok )
      _delayed_queue.pop();

    // обертке callback
    // _delayed_queue.pop();
  }

  void process_non_sequence_queue(std::unique_lock<mutex_type>& lk)
  {
    delayed_queue dq;
    dq.swap(_delayed_queue);
    lk.unlock();
    
    while ( !dq.empty() )
    {
      dq.front()();
      dq.pop();
    }
    lk.lock();
  }

  void process_queue()
  {
    std::unique_lock<mutex_type> lk(_mutex);

    if ( _delayed_queue.empty() )
      return;
    
    if ( _conf.sequence_mode )
    {
      this->process_sequence_queue(lk);
    }
    else
    {
      this->process_non_sequence_queue(lk);  
    }
  }

  // Когда вызывать, определяеться в method_list
  void startup(size_t clinet_id, interface_ptr ptr )
  {
    std::unique_lock<mutex_type> lk(_mutex);
    
    this->_clients[clinet_id] = ptr;
    this->_client_count = this->_clients.size();
    this->_cli_itr = this->_clients.begin();
    lk.unlock();
    this->process_queue();
    lk.lock();
  }
    
  // Когда вызывать, определяеться в method_list
  void shutdown(size_t clinet_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    this->_clients.erase(clinet_id);
    this->_cli_itr = this->_clients.begin();
    this->_client_count = this->_clients.size();
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
    
    if ( !_conf.enabled || _clients.empty() )
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
  
  template<typename Resp>
  std::function<void(Resp)> wrap_callback(std::function<void(Resp)>&& callback) 
  {
    if (_conf.sequence_mode)
    {
      return [this, callback](Resp req)
      {
        if ( callback!=nullptr)
          callback( std::move(req) );
        this->process_queue();
      };
    }
    else
    {
      return std::move(callback);
    }
  }

  template<typename Resp>
  std::function<void(size_t, Resp)> wrap_callback2(std::function<void(size_t, Resp)>&& callback) 
  {
    
    /*if ( callback==nullptr)
      return nullptr;
      */
    
    if (_conf.sequence_mode)
    {
      return [this, callback](size_t io_id, Resp req)
      {
        if ( callback!=nullptr)
          callback( io_id, std::move(req) );
        this->process_queue();
      };
    }
    else
    {
      return std::move(callback);
    }
  }

  template<
    typename Req, 
    typename Resp,
    typename... Args
  >
  std::function<bool()> wrap( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(Resp)> callback, 
    Args... args
  )
  {
    // Ахтунг! То-же и для wrap2!!!
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
        //this->post(mem_ptr, std::move(*preq), callback, args...);
        
        if ( auto cli = this->get().lock() )
        {
          (cli.get()->*mem_ptr)( std::move(*preq), std::move(callback), args... );
          return true;
        }
        else
        {
          return false;
        }

      }, 
      mem_ptr, 
      preq, 
      this->wrap_callback( std::move(callback) ), 
      std::forward<Args>(args)...
    );
  }

  template<
    typename Req, 
    typename Resp,
    typename... Args
  >
  std::function<bool()> wrap2( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(size_t, Resp)> callback, 
    Args... args
  )
  {
    auto preq = std::make_shared<Req>( std::move(req) );
    auto wcallback =       [this](
        void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
        std::shared_ptr<Req> preq, 
        std::function<void(size_t, Resp)> callback, 
        Args... args
      ) -> bool
      {
      
        size_t client_id = 0;
        if ( auto cli = this->get(client_id).lock() )
        {
          // (cli.get()->*mem_ptr)( std::move(*preq), std::move(callback), args... );
          
                (cli.get()->*mem_ptr)( 
        std::move(*preq), 
        [client_id, callback](Resp resp)
        {
          callback( client_id, std::move(resp));
        }, 
        args... 
      );

          return true;
        }
        else
        {
          return false;
        }

        //this->post2(mem_ptr, std::move(*preq), callback, args...);
      };
    // обход бага завата Args...
    return std::bind(
      /*[this](
        void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
        std::shared_ptr<Req> preq, 
        std::function<void(size_t, Resp)> callback, 
        Args... args
      ) -> bool
      {
                
        if ( auto cli = this->get().lock() )
        {
          (cli.get()->*mem_ptr)( std::move(*preq), std::move(callback), args... );
          return true;
        }
        else
        {
          return false;
        }

        //this->post2(mem_ptr, std::move(*preq), callback, args...);
      }*/
      wcallback, 
      mem_ptr, 
      preq, 
      this->wrap_callback2( std::move(callback) ), 
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
#error не правильно очередь разгребает
    DEBUG_LOG_BEGIN(size_t(this) <<  "---> post  callback==nullptr? " << (callback==nullptr) )
    auto cli = this->get().lock();

    std::lock_guard<mutex_type> lk(_mutex);

    if ( !_conf.enabled )
      return;
    
    if ( _conf.sequence_mode && ( _wait_flag || cli == nullptr || !_delayed_queue.empty() )  )
    {
      _delayed_queue.push( this->wrap( mem_ptr, std::move(req), callback, std::forward<Args>(args)...) );
      
    }
    else if ( cli!=nullptr )
    {
       
      if ( _conf.sequence_mode )
      {
        auto tmp = std::move(callback);
        callback = this->wrap_callback( std::move(tmp) );
        _wait_flag = true;
      }
      _mutex.unlock();
      DEBUG_LOG_END( size_t(this) << "---> post send callback==nullptr? " << (callback==nullptr) )
      (cli.get()->*mem_ptr)( std::move(req), callback, args... );
      _mutex.lock();
    }
    DEBUG_LOG_END( size_t(this) << "---> post  callback==nullptr? " << (callback==nullptr) )
    /*
    // TODO: что будет если клиент закрылся после получения this->get()
    bool ready_for_send = cli!=nullptr && _delayed_queue.empty();

    if ( ready_for_send )
    {
      DEBUG_LOG_MESSAGE("provider post -2-")
      if ( _conf.sequence_mode )
      {
        DEBUG_LOG_MESSAGE("provider post -3-")
        _delayed_queue.push(nullptr);
        callback = this->wrap_callback( std::move(callback) );
      }
      DEBUG_LOG_MESSAGE("provider post -4-")
      _mutex.unlock();
      (cli.get()->*mem_ptr)( std::move(req), callback, args... );
      _mutex.lock();
      DEBUG_LOG_MESSAGE("provider post -5-")
    }
    else
    {
      DEBUG_LOG_MESSAGE("provider post -6-")
      if ( _conf.sequence_mode && _delayed_queue.empty() )
      {
        DEBUG_LOG_MESSAGE("provider post -7-")
        _delayed_queue.push(nullptr);  
      }
      DEBUG_LOG_MESSAGE("provider post -8-")
      _delayed_queue.push( this->wrap( mem_ptr, std::move(req), callback, std::forward<Args>(args)...) );
    }
    */
    

    
    /*
    if ( !_conf.enabled )
      return;
    
    if (auto cli = this->get().lock() )
    {
      (cli.get()->*mem_ptr)( std::move(req), callback, args... );
    }
    else
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _delayed_queue.push( this->wrap( mem_ptr, std::move(req), callback, std::forward<Args>(args)...) );
    }
    */
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
    if ( !_conf.enabled )
      return;

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
  
  size_t queue_size() const
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return _clients.size();
  }
  
private:
  bool _wait_flag;
  provider_config _conf;
  //std::atomic<bool> _sequence_mode;
  std::atomic<size_t>  _client_count;
  mutex_type _mutex;
  clinet_map _clients;
  typename clinet_map::iterator _cli_itr;
  delayed_queue _delayed_queue;
  shudown_handler_list _shudown_handlers;
};
  
}}

