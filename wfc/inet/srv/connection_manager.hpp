#pragma once

#include <wfc/inet/conn/iconnection.hpp>

#include <set>
#include <mutex>
#include <memory>


namespace wfc{ namespace inet{
  
class connection_manager
{
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<iconnection> connection_ptr;
  //virtual boost::asio::ip::address remote_address() = 0;
  //virtual unsigned short remote_port() = 0;
  typedef std::pair<boost::asio::ip::address, unsigned short> endpoint_type;
  
  typedef std::map<endpoint_type,  connection_ptr> by_endpoint_map;
  typedef std::map<time_t,  connection_ptr> by_activity_map;
  typedef std::set< connection_ptr > connection_set;
public:
  bool insert(connection_ptr conn)
  {
    endpoint_type ep( conn->remote_address(), conn->remote_port());
    std::lock_guard<mutex_type> lk(_mutex);
    
    auto ep_itr = _by_endpoint.find( ep );
    if (ep_itr != _by_endpoint.end() )
      return false; // TODO:

    _by_activity.insert( std::make_pair(time(0),  conn) );
    _by_endpoint.insert( std::make_pair(ep,  conn) );
    _connections.insert(conn);
    return true;
  }

  void erase(connection_ptr conn)
  {
#error
    std::lock_guard<mutex_type> lk(_mutex);
    _connections.erase(conn);
  }
  
  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    for (auto& ptr : _connections )
      ptr->close();
    _connections.clear();
  }
  
private:
  connection_set  _connections;
  by_endpoint_map _by_endpoint;
  by_activity_map _by_activity;
  std::mutex     _mutex;
};

}}
