#pragma once

#include <wfc/gateway/provider_config.hpp>
#include <wfc/thread/rwlock.hpp>
#include <wfc/thread/spinlock.hpp>
#include <memory>
#include <map>
#include <list>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class provider_base
{
public:
  typedef Itf interface_type;
  typedef std::shared_ptr<interface_type> interface_ptr;
  
  typedef ::wfc::spinlock basic_mutex_type;
  typedef ::wfc::rwlock<basic_mutex_type> mutex_type;

  typedef std::map<size_t, interface_ptr> client_map;
  typedef typename client_map::const_iterator client_iterator;
  
  typedef std::function<void(size_t)> shudown_handler;
  typedef std::list<shudown_handler> shudown_handler_list;

  provider_base(const provider_config& conf)
  {
    this->reconfigure(conf);
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
  virtual void startup(size_t clinet_id, std::shared_ptr<interface_type> ptr )
  {
    std::unique_lock<mutex_type> lk(_mutex);
    
    this->_clients[clinet_id] = ptr;
    this->_cli_itr = this->_clients.begin();
  }
    
  // Когда вызывать, определяеться в method_list
  virtual void shutdown(size_t clinet_id)
  {
    std::unique_lock<mutex_type> lk(_mutex);

    this->_clients.erase(clinet_id);
    this->_cli_itr = this->_clients.begin();
    auto shs = _shudown_handlers;
    lk.unlock();

    for ( auto& sh: shs )
      sh( clinet_id );
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

template<typename Itf, template<typename> class Derived>  
class basic_provider
  : public provider_base<Itf>
  , public std::enable_shared_from_this< Derived<Itf> >
{
  typedef provider_base<Itf> super;
  
  basic_provider(const provider_config& conf)
    : super(conf)
  {
  }
};


}}
