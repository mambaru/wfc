#pragma once

#include <wfc/gateway/provider_config.hpp>
#include <wfc/thread/rwlock.hpp>
#include <wfc/thread/spinlock.hpp>
#include <wfc/logger.hpp>
#include <memory>
#include <map>
#include <list>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class iprovider
{
public:
  typedef Itf interface_type;
  typedef std::shared_ptr<interface_type> interface_ptr;
  typedef std::function<void(size_t)> shudown_handler;
  typedef std::list<shudown_handler> shudown_handler_list;

  
  virtual ~iprovider(){}

  virtual void reconfigure(const provider_config& conf) = 0;
  
  virtual size_t ready_count() const  = 0;
  
  virtual interface_ptr get(size_t& client_id) const  = 0;
  
  virtual interface_ptr get() const  = 0;
  
  virtual void startup(size_t clinet_id, std::shared_ptr<interface_type> ptr )  = 0;

  virtual void shutdown(size_t clinet_id)  = 0;

  virtual void add_shudown_handler( shudown_handler sh )  = 0;
  
  virtual shudown_handler_list get_shudown_handlers( ) const  = 0;

  virtual void set_shudown_handlers( shudown_handler_list lst )  = 0;
};
  
template<typename Itf, typename Mutex = ::wfc::spinlock>  
class provider_base
  : public iprovider<Itf>
{
public:
  typedef iprovider<Itf> super;
  typedef typename super::interface_type interface_type;
  typedef typename super::interface_ptr interface_ptr;
  typedef typename super::shudown_handler shudown_handler;
  typedef typename super::shudown_handler_list shudown_handler_list;
  
  typedef Mutex basic_mutex_type;
  typedef ::wfc::rwlock<basic_mutex_type> mutex_type;
  //typedef std::lock_guard<mutex_type>  lock_guard_type;
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
    // TODO: проверить mode
    _conf = conf;
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
  client_map _clients;
  mutable client_iterator _cli_itr;
};

template<typename Itf, template<typename> class Derived, typename Mutex = /*::wfc::spinlock*/ std::mutex >  
class basic_provider
  : public provider_base<Itf, Mutex>
  , public std::enable_shared_from_this< Derived<Itf> >
{
  
public:
  
  typedef provider_base<Itf, Mutex> super;
  
  basic_provider(const provider_config& conf)
    : super(conf)
  {}
};

}}
