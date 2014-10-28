#pragma once

#include <wfc/provider/provider_config.hpp>
#include <wfc/provider/iprovider.hpp>
#include <wfc/thread/rwlock.hpp>
#include <wfc/logger.hpp>
#include <memory>
#include <map>
#include <list>

namespace wfc{ namespace provider{ 

template<typename Itf, typename Mutex /*= ::std::mutex*/ >  
class provider_base
  : public iprovider<Itf>
{
public:
  typedef iprovider<Itf> super;
  typedef typename super::interface_type interface_type;
  typedef typename super::interface_ptr interface_ptr;
  typedef typename super::shudown_handler shudown_handler;
  typedef typename super::shudown_handler_list shudown_handler_list;
  typedef typename super::startup_handler startup_handler;
  typedef typename super::startup_handler_list startup_handler_list;
  
  typedef Mutex basic_mutex_type;
  typedef ::wfc::rwlock<basic_mutex_type> mutex_type;
  
  typedef ::wfc::read_lock<mutex_type> read_lock_type;

  typedef std::map<size_t, interface_ptr> client_map;
  typedef typename client_map::const_iterator client_iterator;

  provider_base(const provider_config& conf)
  {
    this->reconfigure(conf);
  }
  
  virtual size_t ready_count() const 
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return this->ready_count_();
  }
  
  virtual void reconfigure(const provider_config& conf)
  {
    std::lock_guard<mutex_type> lk(_mutex); 
    _conf = conf;
    switch ( _conf.mode )
    {
      case provider_mode::_auto_: 
        if ( _conf.max_waiting == 1)
        {
          _conf.mode = provider_mode::sequenced;
          if ( conf.queue_limit == 0 )
          {
            _conf.queue_limit = 1024;
          }
        }
        else if ( _conf.max_waiting == 0)
        {
          if ( _conf.queue_limit==0 )
          {
            _conf.mode = provider_mode::simple;
          }
          else
          {
            _conf.mode = provider_mode::connectify;
          }
        }
        else
        {
          _conf.mode = provider_mode::insured;
          if ( _conf.queue_limit==0 )
          {
            _conf.queue_limit=1024;
          }
        }
        break;
      case provider_mode::simple: 
        _conf.queue_limit = 0;
        _conf.max_waiting = 0;
        break;
      case provider_mode::connectify: 
        if ( _conf.queue_limit == 0 )
          _conf.queue_limit = 1024;
        _conf.max_waiting = 0;
        break;
      case provider_mode::insured:
        if ( _conf.queue_limit == 0 )
          _conf.queue_limit = 1024;
        if ( _conf.max_waiting == 0)
          _conf.max_waiting = 1024;
        break;
      case provider_mode::sequenced: 
        if ( _conf.queue_limit == 0 )
          _conf.queue_limit = 1024;
        if ( _conf.max_waiting != 1 )
          _conf.max_waiting = 1;
        break;
    }
    // TODO: в лог текущий режим
    
  }
  
  virtual interface_ptr get(size_t& client_id) const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return this->get_(client_id);
  }

  virtual interface_ptr get() const
  {
    size_t client_id = 0;
    return this->get(client_id);
  }

    // Когда вызывать, определяеться в method_list
  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->startup_(client_id, std::move(ptr));
  }
    
  // Когда вызывать, определяеться в method_list
  virtual void shutdown(size_t client_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->shutdown_(client_id);
  }

  /*
   * Обработчики завершения
   */
  
  virtual void add_shudown_handler( shudown_handler sh )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _shudown_handlers.push_back(sh);
  }
  
  virtual shudown_handler_list get_shudown_handlers( ) const
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return _shudown_handlers;
  }

  virtual void set_shudown_handlers( shudown_handler_list lst )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _shudown_handlers = std::move(lst);
  }

  virtual void add_startup_handler( startup_handler sh )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _startup_handlers.push_back(sh);
  }
  
  virtual startup_handler_list get_startup_handlers( ) const
  {
    ::wfc::read_lock<mutex_type> lk(_mutex);
    return _startup_handlers;
  }

  virtual void set_startup_handlers( startup_handler_list lst )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _startup_handlers = std::move(lst);
  }

protected:
  
  size_t ready_count_() const 
  {
    return this->_clients.size();
  }
  
  void startup_(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    if ( !_conf.enabled )
      return;

    this->_clients[client_id] = ptr;
    this->_cli_itr = this->_clients.begin();
    
    if ( !_startup_handlers.empty() )
    {
      auto shs = _startup_handlers;
      _mutex.unlock();
      for ( auto& sh: shs )
      {
        try 
        { 
          sh( client_id,  ptr); 
        } 
        catch(...)
        {
          DAEMON_LOG_FATAL("provider_base::startup_[startup_handler]: unhandled exception")
          abort();
        }
      }
      _mutex.lock();
    }
  }
  
  void shutdown_(size_t client_id)
  {
    this->_clients.erase(client_id);
    this->_cli_itr = this->_clients.begin();
    if ( !_shudown_handlers.empty() )
    {
      auto shs = _shudown_handlers;
      _mutex.unlock();
      for ( auto& sh: shs )
      {
        try 
        { 
          sh( client_id ); 
        } 
        catch(...)
        {
          DAEMON_LOG_FATAL("provider_base::shutdown_[shudown_handler]: unhandled exception")
          abort();
        }
      }
      _mutex.lock();
    }
  }
  
  interface_ptr get_(size_t& client_id) const
  {
    if ( !_conf.enabled || _clients.empty() )
      return nullptr;
    
    if ( _cli_itr == _clients.end() )
      _cli_itr = _clients.begin();
    client_id = _cli_itr->first;
    
    return (_cli_itr++)->second;
  }

  interface_ptr get_() const
  {
    size_t client_id = 0;
    return this->get_(client_id);
  }

protected:
  provider_config _conf;
  mutable mutex_type _mutex;

private:
  shudown_handler_list _shudown_handlers;
  startup_handler_list _startup_handlers;
  client_map _clients;
  mutable client_iterator _cli_itr;
};

}}
