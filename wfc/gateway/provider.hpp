#pragma once

#include <map>
#include <mutex>
#include <queue>
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
  typedef provider<Itf> self;
  typedef Itf interface_type;
  typedef std::weak_ptr<interface_type> interface_ptr;
  typedef std::map<size_t, interface_ptr> clinet_map;
  
public:
  
  provider()
    : _cli_itr(_clients.end())
  {}
  
  void startup(size_t clinet_id, interface_ptr ptr )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    std::cout << "connected! " << clinet_id << std::endl;
    this->_clients[clinet_id] = ptr;
    this->_cli_itr = this->_clients.begin();
    while ( !_delayed_queue.empty() )
    {
      _delayed_queue.front()();
      _delayed_queue.pop();
    }
  }
    
  void shutdown(size_t clinet_id )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    std::cout << "closed! " << clinet_id << std::endl;
    this->_clients.erase(clinet_id);
    this->_cli_itr = this->_clients.begin();
    /*
    // TODO: send_subscribe для всех подписчиков clinet_id
    auto lst = this->search_(clinet_id);
    pubsub::request::subscribe req;
    for ( auto& l : lst )
    {
      std::cout << "resubscribe: " << l->channel << std::endl;
      req.channel = l->channel;
      this->send_subscribe( std::make_unique<pubsub::request::subscribe>(req), nullptr, l->id, l->handler );
    }
    */
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
  
private:
  
  mutex_type _mutex;
  clinet_map _clients;
  typename clinet_map::iterator _cli_itr;
  std::queue< std::function<void()> > _delayed_queue;


  
};
  
}}

