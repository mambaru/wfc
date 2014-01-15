#pragma once

#include <wfc/inet/conn/iconnection.hpp>
#include <wfc/inet/srv/iactivity.hpp>

#include <set>
#include <mutex>
#include <memory>


namespace wfc{ namespace inet{
  
  
class connection_manager
  : public std::enable_shared_from_this<connection_manager>
  , public iactivity
{
  typedef std::shared_ptr<iconnection> connection_ptr;
  typedef boost::asio::ip::address     address_type;
  typedef unsigned short               port_type;
  
  struct info
  {
    connection_ptr conn;
    address_type   addr;
    port_type      port;
    time_t         ts;
  };
  
  struct by_conn
  {
    bool operator() (const info* left, const info* right) const
    {
      return left->conn < right->conn;
    }
  };
  
  struct by_endpoint
  {
    bool operator() (const info* left, const info* right) const
    {
      return left->port < right->port 
             || (! ( right->port < left->port ) && left->addr < right->addr );
    }
  };
  
  struct by_ts: by_conn
  {
    bool operator() (const info* left, const info* right) const
    {
      return left->ts < right->ts 
            || (! (right->ts < left->ts) && by_conn::operator()(left, right) );
    }
  };
  
  typedef std::mutex mutex_type;
  typedef std::set<info*, by_endpoint> by_endpoint_set;
  typedef std::set<info*, by_ts>       by_ts_set;
  typedef std::set<info*, by_conn >    by_connection_set;
  
  //virtual boost::asio::ip::address remote_address() = 0;
  //virtual unsigned short remote_port() = 0;
  typedef std::pair<boost::asio::ip::address, unsigned short> endpoint_type;
  
  /*
  typedef std::map<endpoint_type,  connection_ptr> by_endpoint_map;
  typedef std::map<time_t,  connection_ptr> by_activity_map;
  typedef std::set< connection_ptr > connection_set;
  */
public:
  
  ~connection_manager()
  {
    std::cout << "~connection_manager()[[" << std::endl;
    std::lock_guard<mutex_type> lk(_mutex);
    this->clear();
    std::cout << "]]~connection_manager()" << std::endl;
  }
  
  virtual void update(connection_ptr conn) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( info* inf = this->find_by_conn(conn) )
    {
      std::cout << "update" << std::endl;
      _by_ts.erase(inf);
      inf->ts = time(0);
      if ( !_by_ts.insert(inf).second )
        abort();
    }
  }
  
  void shutdown_inactive(time_t ts)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    info inf = info{ nullptr , boost::asio::ip::address(), 0, time(0) - ts };
    auto end = _by_ts.lower_bound( &inf );
    auto beg = _by_ts.begin();
    std::cout << "shutdown_inactive " << ts << " " << (beg!=end) << std::endl;
    while (beg!=end)
    {
      (*beg)->conn->shutdown();
      _by_conn.erase(*beg);
      _by_endpoint.erase(*beg);
      _by_ts.erase(beg++);
    }
    
  }
  
  bool insert(connection_ptr conn)
  {
    info* inf = new info{conn, conn->remote_address(), conn->remote_port(), time(0) };
    
    std::lock_guard<mutex_type> lk(_mutex);
 
    bool flag = _by_conn.insert(inf).second;
    if ( flag )
    {
      flag = _by_endpoint.insert(inf).second;
      if ( flag )
      {
        flag = _by_ts.insert(inf).second;
        if ( !flag )
        {
          _by_conn.erase(inf);
          _by_endpoint.erase(inf);
        }
      }
      else
      {
        _by_conn.erase(inf);
      }
    }
    
    if (!flag)
      delete inf;
    
    return flag;
  }

  void erase(connection_ptr conn)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    std::cout << "erase[[" << std::endl;
    this->erase( this->find_by_conn(conn) );
    std::cout << "]]erase" << std::endl;
  }
  
  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    std::cout << "stop[[" << std::endl;
    for (info* inf : _by_conn )
      inf->conn->close();
    this->clear();
    std::cout << "]]stop" << std::endl;
  }
  
private:
  
  void clear()
  {
    if ( _by_conn.size() != _by_endpoint.size() || _by_conn.size() != _by_ts.size())
    {
      abort();
    }
    
    std::cout << "clear[[" << std::endl;
    while ( !_by_conn.empty() )
    {
      this->erase( *(_by_conn.begin()) );
    }
    std::cout << "]]clear" << std::endl;
  }
  
  info* find_by_conn(connection_ptr conn)
  {
    std::cout << "find_by_conn[[" << std::endl;
    info inf = info{ conn , boost::asio::ip::address(), 0, 0 };
    
    auto itr = _by_conn.find( &inf );
    if ( itr == _by_conn.end() )
      return nullptr;
    std::cout << "]]find_by_conn" << std::endl;
    return *itr;
  }
  
  void erase(info* inf)
  {
    std::cout << "erase ptr[[" << std::endl;
    if ( inf == nullptr )
      return;
    
    std::cout << "erase ptr 1" << std::endl;
    _by_conn.erase(inf);
    _by_endpoint.erase(inf);
    _by_ts.erase(inf);
    delete inf;
    std::cout << "]]erase ptr" << std::endl;
  }
  
private:
  
  by_connection_set  _by_conn;
  by_endpoint_set    _by_endpoint;
  by_ts_set          _by_ts;
  std::mutex         _mutex;
};

}}
