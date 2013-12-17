#pragma once

#include <wfc/inet/conn/iconnection.hpp>

#include <set>
#include <mutex>
#include <memory>


namespace wfc{ namespace inet{
  
class connection_manager
{
  typedef std::mutex mutex_type;
public:
  void insert(std::shared_ptr<iconnection> conn)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _connections.insert(conn);
  }

  void erase(std::shared_ptr<iconnection> conn)
  {
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
  std::set< std::shared_ptr<iconnection> > _connections;
  std::mutex _mutex;
};

}}
