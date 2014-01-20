#pragma once

#include <wfc/inet/conn/iconnection.hpp>
#include <wfc/inet/srv/iactivity.hpp>

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
  , public iactivity
{
  //typedef iconnection connection_type;
  
  typedef Conn connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  typedef std::shared_ptr<iconnection> iconnection_ptr;
  typedef boost::asio::ip::address     address_type;
  typedef unsigned short               port_type;
  
  struct info
  {
    connection_ptr  conn;
    iconnection_ptr iconn;
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

  struct by_iconn
  {
    bool operator() (const info* left, const info* right) const
    {
      return left->iconn < right->iconn;
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
  typedef std::set<info*, by_iconn >   by_iconnection_set;
  
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
  
#warning как получить доступ из connection без полиморфизма или сделать индекс по интерфейсу? 

  virtual void update( iconnection_ptr conn) 
  {
    std::cout << "connection_manager::update[[" << std::endl;
    std::lock_guard<mutex_type> lk(_mutex);
    std::cout << "connection_manager::update locked" << std::endl;
    this->update_connection(conn);
    std::cout << "]]connection_manager::update" << std::endl;
  }
  
  connection_ptr least()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _by_ts.empty() )
      return nullptr;
    
    info *inf = *(_by_ts.begin());
    connection_ptr conn = inf->conn;
    this->update_connection(conn);
    return conn;
  }
  
  void shutdown_inactive(time_t ts)
  {
    //std::lock_guard<mutex_type> lk(_mutex);
    std::cout << "shutdown_inactive ... X"<< std::endl;    
    // std::cout << "shutdown_inactive ... " <<  _mutex.try_lock() << std::endl;    
    _mutex.lock();
    std::cout << "shutdown_inactive ... " <<  "locked" << std::endl;    
    info inf = info{ nullptr , nullptr, boost::asio::ip::address(), 0, time(0) - ts };
    auto end = _by_ts.lower_bound( &inf );
    auto beg = _by_ts.begin();
    std::cout << "shutdown_inactive " << ts << " " << (beg!=end) << std::endl;
    while (beg!=end)
    {
      connection_ptr conn = (*(beg++))->conn;
      _mutex.unlock();
      conn->shutdown();
      _mutex.lock();
    }
    std::cout << "shutdown_inactive end" << std::endl;
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
    info* inf = new info{conn, conn, addr, port, time(0) };
    std::lock_guard<mutex_type> lk(_mutex);
    return this->insert(inf);    
  }
  
  bool insert(connection_ptr conn)
  {
    info* inf = new info{conn, conn, conn->remote_address(), conn->remote_port(), time(0) };
    std::lock_guard<mutex_type> lk(_mutex);
    return this->insert(inf);
  }

  void erase(connection_ptr conn)
  {
    std::cout << "erase[[" << _by_conn.size() << std::endl;
    std::lock_guard<mutex_type> lk(_mutex);
    this->erase( this->find_by_conn(conn) );
    std::cout << "]]erase" << std::endl;
  }
  
  void stop()
  {
    //std::lock_guard<mutex_type> lk(_mutex);
    std::cout << "connection_manager stop[[" << std::endl;
    _mutex.lock();
    std::cout << "connection_manager stop A" << std::endl;
    for (info* inf : _by_conn )
    {
      connection_ptr conn = inf->conn;
      std::cout << "connection_manager stop " << (inf==nullptr) << (inf->conn==nullptr)  << std::endl;
      _mutex.unlock();
      conn->close();
      _mutex.lock();
    }
    this->clear();
    _mutex.unlock();

    std::cout << "]]stop connection_manager" << std::endl;
  }
  
  
  
private:
  
  void update_connection(iconnection_ptr conn) 
  {
    if ( info* inf = this->find_by_iconn(conn) )
    {
      std::cout << "update" << std::endl;
      _by_ts.erase(inf);
      inf->ts = time(0);
      if ( !_by_ts.insert(inf).second )
      {
        std::cout << "update abort" << std::endl;
        abort();
      }
    }
  }

  
  bool insert(info* inf)
  {
    bool flag = _by_conn.insert(inf).second;
    if ( flag )
    {
      if ( !_by_iconn.insert(inf).second )
        abort();
      
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
    
    std::cout << "insert " << _by_conn.size() << std::endl;
    
    return flag;

    
  }
  
  void clear()
  {
    if ( _by_conn.size() != _by_endpoint.size() || _by_conn.size() != _by_ts.size())
    {
      std::cout << "clear() abort" << std::endl;
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
    info inf = info{ conn, nullptr, boost::asio::ip::address(), 0, 0 };
    
    auto itr = _by_conn.find( &inf );
    if ( itr == _by_conn.end() )
    {
      std::cout << "find_by_conn not found " << size_t(conn.get()) << std::endl;
      for ( info* p: _by_conn)
      {
        std::cout << size_t(p->conn.get()) << std::endl;
      }
      return nullptr;
    }
    std::cout << "]]find_by_conn" << std::endl;
    return *itr;
  }

  info* find_by_iconn(iconnection_ptr conn)
  {
    std::cout << "find_by_conn[[" << std::endl;
    info inf = info{ nullptr, conn, boost::asio::ip::address(), 0, 0 };
    
    auto itr = _by_iconn.find( &inf );
    if ( itr == _by_iconn.end() )
    {
      std::cout << "find_by_conn not found " << size_t(conn.get()) << std::endl;
#warning УДАЛИТЬ
      for ( info* p: _by_iconn)
      {
        std::cout << size_t(p->conn.get()) << std::endl;
      }
      return nullptr;
    }
    std::cout << "]]find_by_conn" << std::endl;
    return *itr;
  }

  info* find_by_endpoint(address_type addr, port_type port)
  {
    std::cout << "find_by_endpoint[[" << port << std::endl;
    info inf = info{ nullptr , nullptr, addr, port, 0 };
    
    auto itr = _by_endpoint.find( &inf );
    if ( itr == _by_endpoint.end() )
      return nullptr;
    std::cout << (*itr)->port <<  "]]find_by_endpoint" << std::endl;
    return *itr;
  }

  void erase(info* inf)
  {
    std::cout << "erase ptr[[" << std::endl;
    if ( inf == nullptr )
    {
      std::cout << "erase ptr not found" << std::endl;
      return;
    }
    
    std::cout << "erase ptr 1" << std::endl;
    _by_conn.erase(inf);
    _by_iconn.erase(inf);
    std::cout << "erase ptr 1.1" << std::endl;
    _by_endpoint.erase(inf);
    std::cout << "erase ptr 1.2" << std::endl;
    _by_ts.erase(inf);
    std::cout << "erase ptr 1.3" << std::endl;
    delete inf;
    std::cout << "]]erase ptr" << std::endl;
  }
  
private:
  
  by_connection_set  _by_conn;
  by_iconnection_set _by_iconn;
  by_endpoint_set    _by_endpoint;
  by_ts_set          _by_ts;
  mutex_type         _mutex;
};

}}
