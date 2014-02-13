#pragma once


#include <set>
#include <mutex>
#include <memory>


namespace wfc{ namespace inet{
  
template< template<typename> class Manager>
struct connection_manager_type
{
  typedef connection_manager_type<Manager> self;
  typedef fas::metalist::advice metatype;
  typedef _connection_manager_type_ tag;
  typedef self advice_class;
  
  connection_manager_type()
  {
    // TODO: operator()(tag<_startup_>)
    //_manager = std::make_shared< Manager<Conn> >();
  }
  
  template<typename Conn>
  struct apply
  {
    typedef Manager<Conn> type;
  };
  
  
  template<typename Conn, typename T>
  std::shared_ptr< Manager<Conn> > create(T& t)
  {
    return std::make_shared< Manager<Conn> >();
  }
  
private:
  // std::shared_ptr< Manager<Conn> > _manager;
};

template<typename Conn /*= fas::empty_type*/ >
class connection_manager
  : public std::enable_shared_from_this<connection_manager<Conn> >
  //, public iactivity
{
  //typedef iconnection connection_type;
  
  typedef Conn connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  //typedef std::shared_ptr<iconnection> iconnection_ptr;
  typedef boost::asio::ip::address     address_type;
  typedef unsigned short               port_type;
  
  struct info
  {
    connection_ptr  conn;
    //iconnection_ptr iconn;
    address_type    addr;
    port_type       port;
    time_t          ts;
  };
  
  struct by_conn
  {
    bool operator() (const info* left, const info* right) const
    {
      return left->conn < right->conn;
    }
  };

  /*
  struct by_iconn
  {
    bool operator() (const info* left, const info* right) const
    {
      return left->iconn < right->iconn;
    }
  };*/
  
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
  
  // TODO: убрать mutex, сделать через strad
  typedef std::mutex mutex_type;
  typedef std::set<info*, by_endpoint> by_endpoint_set;
  typedef std::set<info*, by_ts>       by_ts_set;
  typedef std::set<info*, by_conn >    by_connection_set;
  //typedef std::set<info*, by_iconn >   by_iconnection_set;
  
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
    std::lock_guard<mutex_type> lk(_mutex);
    this->clear();
  }
  
  connection_manager()
  {
    
  }
  
  connection_manager(const connection_manager&) = delete;
  
  /*
  virtual void update( iconnection_ptr conn) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->update_connection(conn);
  }
  */

  
  void update2( connection_ptr conn) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->update_connection2(conn);
  }
  

  connection_ptr least()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _by_ts.empty() )
      return nullptr;
    
    info *inf = *(_by_ts.begin());
    connection_ptr conn = inf->conn;
    this->update_connection2(conn);
    return conn;
  }
  
  void shutdown_inactive(time_t ts)
  {
    _mutex.lock();
    info inf = info{ nullptr,  boost::asio::ip::address(), 0, time(0) - ts };
    auto end = _by_ts.lower_bound( &inf );
    auto beg = _by_ts.begin();
    while (beg!=end)
    {
      info *inf = *(beg++);
      connection_ptr conn = inf->conn;
      this->on_wait( inf );
      _mutex.unlock();
      conn->shutdown();
      _mutex.lock();
    }
    
    _mutex.unlock();
  }
  
  connection_ptr find(address_type addr, port_type port)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( info* inf = this->find_by_endpoint(addr, port) )
    {
      return inf->conn;
    }
    return nullptr;
  }
  
  bool insert(connection_ptr conn, address_type addr, port_type port)
  {
    conn->get_aspect().template get< conn::_activity_>()=[this, conn]()
    {
      this->update2(conn); 
    };
    
    info* inf = new info{conn, addr, port, time(0) };
    std::lock_guard<mutex_type> lk(_mutex);
    return this->insert(inf);    
  }
  
  bool insert(connection_ptr conn)
  {
    
    conn->get_aspect().template get<conn::_activity_>()=[this, conn]()
    {
      this->update2(conn); 
    };
    info* inf = new info{conn, conn->remote_address(), conn->remote_port(), time(0) };
    std::lock_guard<mutex_type> lk(_mutex);
    return this->insert(inf);
  }

  void erase(connection_ptr conn)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    std::cout << "erase0 " << conn.use_count() << std::endl;
    conn->get_aspect().template get<conn::_activity_>() = nullptr; // Ахтунг, lambda захватывает pconn
    std::cout << "erase1 " << conn.use_count() << std::endl;
    this->erase( this->find_by_conn(conn) );
    std::cout << "erase2 " << conn.use_count()  << " " << _wait_list.size() << " " << _by_ts.size() << std::endl;
    
  }
  
  void stop()
  {
    _mutex.lock();
    for (info* inf : _by_conn )
    {
      connection_ptr conn = inf->conn;
      _mutex.unlock();
      conn->close();
      _mutex.lock();
    }
    this->clear();
    _mutex.unlock();
  }
  
  
  
private:
  
  /*
  void update_connection(iconnection_ptr conn) 
  {
    if ( info* inf = this->find_by_iconn(conn) )
    {
      _by_ts.erase(inf);
      inf->ts = time(0);
      if ( !_by_ts.insert(inf).second )
      {
        abort();
      }
    }
  }*/

  
  void update_connection2(connection_ptr conn) 
  {
    if ( info* inf = this->find_by_conn(conn) )
    {
      _by_ts.erase(inf);
      inf->ts = time(0);
      if ( !_by_ts.insert(inf).second )
      {
        abort();
      }
    }
  }

  
  bool insert(info* inf)
  {
    bool flag = _by_conn.insert(inf).second;
    if ( flag )
    {
      /*if ( !_by_iconn.insert(inf).second )
        abort();*/
      
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
    {
      delete inf;
      abort();
    }
    
    return flag;
  }
  
  void clear()
  {
    if ( _by_conn.size() != _by_endpoint.size() || _by_conn.size() != _by_ts.size())
    {
      abort();
    }
    
    while ( !_by_conn.empty() )
    {
      this->erase( *(_by_conn.begin()) );
    }
  }
  
  info* find_by_conn(connection_ptr conn)
  {
    info inf = info{ conn, boost::asio::ip::address(), 0, 0 };
    
    auto itr = _by_conn.find( &inf );
    if ( itr == _by_conn.end() )
    {
      itr =  _wait_list.find( &inf );
      if ( itr == _by_conn.end() )
      {
        abort();
      }
    }
    return *itr;
  }

  /*
  info* find_by_iconn(iconnection_ptr conn)
  {
    info inf = info{ nullptr, conn, boost::asio::ip::address(), 0, 0 };
    
    auto itr = _by_iconn.find( &inf );
    if ( itr == _by_iconn.end() )
    {
      return nullptr;
    }
    return *itr;
  }
  */

  info* find_by_endpoint(address_type addr, port_type port)
  {
    info inf = info{ nullptr, addr, port, 0 };
    
    auto itr = _by_endpoint.find( &inf );
    if ( itr == _by_endpoint.end() )
      return nullptr;
    return *itr;
  }
  
  void on_wait(info* inf)
  {
    
    _wait_list.insert(inf);
    _by_conn.erase(inf);
    //_by_iconn.erase(inf);
    _by_endpoint.erase(inf);
    _by_ts.erase(inf);
  }

  void erase(info* inf)
  {
    if ( inf == nullptr )
    {
      return;
    }
    
    _by_conn.erase(inf);
    //_by_iconn.erase(inf);
    _by_endpoint.erase(inf);
    _by_ts.erase(inf);
    _wait_list.erase(inf);
    delete inf;
  }
  
private:
  
  by_connection_set  _by_conn;
  //by_iconnection_set _by_iconn;
  by_endpoint_set    _by_endpoint;
  by_ts_set          _by_ts;
  by_connection_set  _wait_list;
  mutex_type         _mutex;
};

}}
