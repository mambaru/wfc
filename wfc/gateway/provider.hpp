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
  
  provider(const provider& ) = delete;
  
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
    if ( _wait_flag )
      return;
      /*abort();
       * */
    DEBUG_LOG_MESSAGE("process_sequence_queue: _delayed_queue.size()==" << _delayed_queue.size());
    _delayed_queue.pop();
    if ( _delayed_queue.empty() )
    {
      DEBUG_LOG_MESSAGE("process_sequence_queue: очередь пуста");
      return;
    }
    auto fn = _delayed_queue.front();
    if ( fn == nullptr )
    {
      DEBUG_LOG_MESSAGE("process_sequence_queue: fn == nullptr");
      abort();
    }
    _wait_flag = true;
    lk.unlock();
    fn();
    lk.lock();
    /*
    auto fn = _delayed_queue.front();
    lk.unlock();
    bool ok = false;
    while ( this->ready() && !ok )
      ok = fn();
    lk.lock();
    if ( ok )
      _delayed_queue.pop();
    */

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
    if ( !this->ready() )
      return;
    
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
    DEBUG_LOG_MESSAGE("provider startup clinet_id=" << clinet_id)
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
  std::function<void(Resp)> wrap_callback(std::function<void(Resp)> callback) 
  {
    if (_conf.sequence_mode)
    {
      return [this, callback](Resp req)
      {
        DEBUG_LOG_MESSAGE("Получен ответ!");
        DEBUG_LOG_MESSAGE("wrap_callback this->_delayed_queue.size()==" << this->_delayed_queue.size());
        if ( callback!=nullptr)
        {
          DEBUG_LOG_BEGIN("callback " << (req!=nullptr));
          callback( std::move(req) );
          DEBUG_LOG_END("callback");
        }
        else
        {
          DEBUG_LOG_MESSAGE("callback==nullptr!");
        }
        _wait_flag = false;
        DEBUG_LOG_MESSAGE("Разбор очереди!");
        this->process_queue();
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
    
    /*if ( callback==nullptr)
      return nullptr;
      */
    
    if (_conf.sequence_mode)
    {
      return [this, callback](size_t io_id, Resp req)
      {
        DEBUG_LOG_MESSAGE("wrap_callback2. Получен ответ!");
        DEBUG_LOG_MESSAGE("wrap_callback2 this->_delayed_queue.size()==" << this->_delayed_queue.size());
        if ( callback!=nullptr)
        {
          DEBUG_LOG_BEGIN("wrap_callback2.callback " << (req!=nullptr));
          callback( io_id, std::move(req) );
          DEBUG_LOG_END("wrap_callback2.callback " << (req!=nullptr));
        }
        _wait_flag = false;
        this->process_queue();
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
  std::function<bool()> wrap( 
    void (interface_type::*mem_ptr)(Req, std::function<void(Resp)>, Args... args), 
    Req req, 
    std::function<void(Resp)> callback, 
    Args... args
  )
  {
    DEBUG_LOG_MESSAGE("################### wrap " << (callback==nullptr) );
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
        DEBUG_LOG_MESSAGE("Отправляем запрос! callback==nullptr? " << (callback==nullptr) );
        //this->post(mem_ptr, std::move(*preq), callback, args...);
        
        if ( auto cli = this->get().lock() )
        {
          DEBUG_LOG_BEGIN("Отправляем запрос! Готов для отправки callback==nullptr? " << (callback==nullptr));
          (cli.get()->*mem_ptr)( std::move(*preq), callback, args... );
          DEBUG_LOG_END("Отправляем запрос! Готов для отправки callback==nullptr? " << (callback==nullptr));
          return true;
        }
        else
        {
          _wait_flag = false;
          return false;
        }

      }, 
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
          // (cli.get()->*mem_ptr)( std::move(*preq), callback, args... );
          //_wait_flag = true;
          
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
          _wait_flag = false;
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
          (cli.get()->*mem_ptr)( std::move(*preq), callback, args... );
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
    DEBUG_LOG_MESSAGE("####################### POST callback==nullptr? " << (callback==nullptr) );
    auto cli = this->get().lock();

    std::lock_guard<mutex_type> lk(_mutex);
    if ( !_conf.enabled )
      return;

    auto fn = this->wrap( mem_ptr, std::move(req), callback, std::forward<Args>(args)...);
    
    if ( _conf.sequence_mode )
    {
      bool ready_for_call = _delayed_queue.empty() && !_wait_flag;
      _delayed_queue.push( fn );
      DEBUG_LOG_MESSAGE("post ready_for_call=" << ready_for_call << " _delayed_queue.size()==" << _delayed_queue.size() );
      if ( ready_for_call )
      {
        _wait_flag = true;
        _mutex.unlock();
        fn();
        _mutex.lock();
      }
    }
    else
    {
      if (auto cli = this->get().lock() )
      {
        _mutex.unlock();
        fn();
        _mutex.lock();
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
    

    DEBUG_LOG_MESSAGE("####################### POST2 callback==nullptr? " << (callback==nullptr) );
    
    auto cli = this->get().lock();

    std::lock_guard<mutex_type> lk(_mutex);
    if ( !_conf.enabled )
      return;

    auto fn = this->wrap2( mem_ptr, std::move(req), callback, std::forward<Args>(args)...);
    
    if ( _conf.sequence_mode )
    {
      bool ready_for_call = _delayed_queue.empty() && !_wait_flag;
      _delayed_queue.push( fn );
      if ( ready_for_call )
      {
        _wait_flag = true;
        _mutex.unlock();
        fn();
        _mutex.lock();
      }
    }
    else
    {
      if (auto cli = this->get().lock() )
      {
        _mutex.unlock();
        fn();
        _mutex.lock();
      }
      else
      {
        _delayed_queue.push( fn );
      }
    }

    
    /*
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
    */
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
  mutex_type _mutex;
  clinet_map _clients;
  typename clinet_map::iterator _cli_itr;
  shudown_handler_list _shudown_handlers;
};
  
}}

