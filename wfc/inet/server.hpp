#pragma once

#include <wfc/inet/connection.hpp>
#include <wfc/inet/iconnection.hpp>
#include <wfc/inet/server_config.hpp>
#include <wfc/memory.hpp>
#include <boost/asio.hpp>
#include <set>
#include <thread>

namespace wfc{ namespace inet{
  
/*
template<typename Tg, typename T>
class type
{
public:
  typedef metalist::advice metatype;
  typedef Tg tag;
  typedef T advice_class;
};

*/
//template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
  
/*
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class tcp_connection_base final
*/


template<typename V>
struct context: 
  ::fas::value<_context_, V>
{
  
};

/*
template<template<typename> class ConnBase= ::fas::aspect_class >
struct connection_base
{
  typedef fas::metalist::advice metatype;
  typedef _connection_base_ tag;

  template<typename A>
  using advice_class = ConnBase<A>;
};
*/

template<template<typename> class ConnBase= ::fas::aspect_class >
struct connection_base_t
{
  template<typename A>
  using type = ConnBase<A>;
};


template<template<typename> class ConnBase= ::fas::aspect_class >
struct connection_base
  : fas::type<_connection_base_, connection_base_t<ConnBase> >
{};
/*{
  typedef fas::metalist::advice metatype;
  typedef _connection_base_ tag;

  template<typename A>
  using advice_class = ConnBase<A>;
};
*/

template<
  template<
    typename, 
    template<typename> class
  > class Conn
>
struct connection_t
{
  template<typename A, template<typename> class AspectClass >
  using type = Conn<A, AspectClass>;
};

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

/*
template<template<typename> class ConnMgr = connection_manager_impl >
struct connection_manager_t
{
  template<typename A>
  using type = ConnMgr<A>;
};

template<template<typename> class ConnMgr = connection_manager_impl >
struct connection_manager
  : fas::type<_connection_manager_, connection_manager_t<ConnMgr> >
{};
*/



template<
  template<
    typename, 
    template<typename> class
  > class Conn
>
struct connection  
  : fas::type<_connection_, connection_t<Conn> >
{};



/*
template<
  template<
    typename, 
    template<typename> class
  > class Conn
>
struct connection
{
  typedef fas::metalist::advice metatype;
  typedef _connection_ tag;
  
  template<typename A, template<typename> class AspectClass >
  using advice_class = Conn<A, AspectClass>;
};
*/
template<typename A>
struct connection_aspect
{
  typedef fas::metalist::advice metatype;
  typedef _connection_aspect_ tag;
  typedef A advice_class;
};

template<typename A>
struct server_aspect
{
  typedef fas::metalist::advice metatype;
  typedef _server_aspect_ tag;
  typedef A advice_class;
};

  


struct context_server
{
  std::string host;
  std::string port;
  int listen_threads;
  
  context_server()
    : listen_threads(0)
  {}
};

struct context_connection
{
  bool enable_stat;
};

class work_thread
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef std::function<void(socket_type)> func_type;
  std::map<int, socket_type> _sockets;

public:
  // TODO: stop
  void start()
  {
    _thread = std::thread([this](){
      std::cout <<  "work_thread start" <<  std::endl;
      // TODO: как выходить? ? ? 
      ::boost::asio::io_service::work work(this->_io_service);
      ///!!! вернуть this->_io_service.run();
      for (;;)
      {
        this->_io_service.run_one();
        std::cout <<  "work_thread run_one()" <<  std::endl;

      }
      std::cout <<  "work_thread stop" <<  std::endl;
    });
  }

  void post(socket_type sock , func_type fun)
  {
    ++_post_counter;
    std::cout <<  "work_thread post 1" <<  std::endl;

    socket_type::native_handle_type fd = sock.native_handle();
    auto itr = _sockets.insert( std::make_pair(fd, std::move(sock))).first;

    ::boost::asio::ip::tcp::endpoint::protocol_type protoc = itr->second.local_endpoint().protocol();

    _io_service.post([this, fd, protoc, fun]() 
    {
      std::cout <<  "work_thread post 3 exec 1 handle = " << fd <<  std::endl;
      socket_type sock(this->_io_service, protoc,  fd);
      std::cout <<  "work_thread post 3 exec 2" <<  std::endl;
      // TODO: Release from _sockets при закрытии 
      // Хуй знает как 
      fun( std::move(sock) );
      std::cout <<  "work_thread post 3 exec 3" <<  std::endl;
      --(this->_post_counter);

    });

    
    /*auto itr = *///_sockets.insert( std::move(sock) ).first;
    /*
    ::boost::asio::ip::tcp::endpoint::protocol_type protocol = sock.local_endpoint().protocol();
    socket_type::native_handle_type native_handle = sock.native_handle();
    std::unique_ptr<socket_type> sock2 = std::make_unique<socket_type>(_io_service, protocol,  native_handle);
    */
   
   /*!
   std::pair<
     ::boost::asio::ip::tcp::endpoint::protocol_type, 
     socket_type::native_handle_type
    > pp(
      itr->local_endpoint().protocol(), 
      fd
      );

    std::cout <<  "work_thread post 2" <<  std::endl;

    _io_service.post([this, pp, fun]() 
    {
      std::cout <<  "work_thread post 3 exec 1 handle = " << pp.second <<  std::endl;
      socket_type sock(this->_io_service, pp.first, pp.second);
      std::cout <<  "work_thread post 3 exec 2" <<  std::endl;      
      fun( std::move(sock) );
      std::cout <<  "work_thread post 3 exec 2" <<  std::endl;

    });
    */
  }
private:
  std::atomic<int> _post_counter;
  ::boost::asio::io_service _io_service;
  std::thread _thread;
};

struct ad_new_connection
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef std::function<void(socket_type)> func_type;
  
  /// initialize
  template<typename T>
  void operator()(T& , ::boost::asio::io_service& /*io*/)
  {
    std::cout <<  "ad_new_connection initialize" <<  std::endl;
    _work_thread.start();
  }

  
  template<typename T>
  void operator()(T&, socket_type sock , func_type fun)
  {
    //std::shared_ptr<socket_type> tmp(std::move(sock));
    //std::cout <<  "ad_new_connection" <<  std::endl;
    //fun( std::move(sock) );
    _work_thread.post( std::move(sock),  fun);
  }

private:
  // TODO: массив (пул воркеров)
  work_thread _work_thread;
};

class accept_thread
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;

public:
  
  accept_thread(::boost::asio::io_service& io)
    : _work(io)
    , _socket(io)
    , _acceptor(nullptr)
  {
    
  }

  accept_thread(const accept_thread&) = delete;
  accept_thread& operator=(const accept_thread&) = delete;
  
  template<typename T>
  void start(T& t, acceptor_type* acceptor)
  {
    _acceptor = acceptor;
    typedef typename T::connection_type connection_type;
    _thread = std::thread([this, &t](){
        std::cout << "thread start " << std::this_thread::get_id() << std::endl;
      connection_manager mgr;
      for(;;)
      {
        std::cout << "thread for(;;) " << std::this_thread::get_id() << std::endl;
        //sleep(10);
        boost::system::error_code ec;
        this->_acceptor->accept( this->_socket, ec);
        std::cout << "thread accept ready " << std::this_thread::get_id() << std::endl;
        if (!this->_acceptor->is_open())
        {
          std::cout << "thread accept not open " << std::endl;
          return;
        }
        if (!ec)
        {
          t.get_aspect().template get<_new_connection_>()(t, std::move( this->_socket), [&t, &mgr](socket_type sock) 
          {
            std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
            pconn->context() = t.connection_context();
            mgr.insert(pconn);
            pconn->start( std::move( sock ), [&mgr](std::shared_ptr<connection_type> pconn)->void
            {
              std::cout << "connection close " << std::endl;
              mgr.erase(pconn);
            });
          });
          /*
          // Connection creator _new_connection_
          std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
          pconn->context() = t.connection_context();
          std::shared_ptr<iconnection> iconn = pconn;
          mgr.insert(pconn);
          
          std::cout << "connection start " << std::endl;
          // TODO: безопасный callback (aka callback_owner)
          pconn->start( std::move( this->_socket ), [&mgr](std::shared_ptr<connection_type> pconn)->void
          {
            std::cout << "connection close " << std::endl;
            mgr.erase(pconn);
          });*/
        }
        else
          std::cout << "thread accept error: " << ec.message() << std::endl;
          
      }
    });
    //sleep(1);
  }


private:  
  ::boost::asio::io_service::work _work;
  socket_type _socket;
  acceptor_type* _acceptor;
  std::thread _thread;
};

struct ad_acceptor
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;
  
  ad_acceptor()
  {
    
  }
  
  template<typename T>
  void operator()(T& , ::boost::asio::io_service& io)
  {
    _io_service = &io;
    _acceptor = std::make_unique<acceptor_type>(io);
    _socket = std::make_unique<socket_type>(io);
    /* initialize */
    /*
    _acceptor = std::make_unique<acceptor_type>(io);
    _acceptor2 = std::make_unique<acceptor_type>(io);
    */
  }
  
  template<typename T>
  void operator()(T& t)
  {
    /* start */
    boost::asio::ip::tcp::resolver resolver( _acceptor->get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      t.server_context().host, 
      t.server_context().port
    });
    
    
    _acceptor->open(endpoint.protocol());
    _acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor->bind(endpoint);
    _acceptor->listen();
    
    int threads = t.server_context().listen_threads;
    for (int i=0; i< threads; ++i)
    {
      _accept_threads.push_back( std::make_unique<accept_thread>(*_io_service) );
      _accept_threads.back()->start(t, &(*_acceptor) );
    }
    
    if (threads==0)
      this->do_accept(t);
  }
  
  template<typename T>
  void do_accept(T& t)
  {
    std::cout <<  "single do accept " <<  std::endl;
    _acceptor->async_accept(
      *_socket,
      [this, &t](boost::system::error_code ec)
      {
        typedef typename T::connection_type connection_type;
        
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!_acceptor->is_open())
        {
          return;
        }

        if (!ec)
        {
          std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
          pconn->context() = t.connection_context();
          std::shared_ptr<iconnection> iconn = pconn;
          _connection_manager.insert(pconn);
          
          // TODO: безопасный callback (aka callback_owner)
          pconn->start( std::move( *(this->_socket) ), [this](std::shared_ptr<connection_type> pconn)->void{
            _connection_manager.erase(pconn);
          });
        }
        else
          std::cout <<  "single do accept error " <<  ec.message() <<  std::endl;

        this->do_accept(t);
        
      } // callback
    ); // async_accept
  }

  
private:
  std::unique_ptr<socket_type> _socket;
  ::boost::asio::io_service* _io_service;
  std::list< std::unique_ptr<accept_thread> > _accept_threads;
  //std::unique_ptr<accept_thread> _accept_thread2;
  
  std::unique_ptr< acceptor_type > _acceptor;
  connection_manager _connection_manager;
  /*
  std::unique_ptr< acceptor_type > _acceptor2;
  */
  
  ///std::set< std::shared_ptr<iconnection> > _connections;
};


struct aspect_server: fas::aspect< fas::type_list_n<
  context<context_server>,
  fas::advice<_new_connection_,  ad_new_connection>, 
  fas::advice<_start_, ad_acceptor>,
  fas::group<_initialize_,  _start_>, 
  fas::group<_initialize_,  _new_connection_>
>::type>
{
};

struct aspect_connection: fas::aspect< fas::type_list_n<
  context<context_connection>
>::type>
{
};


struct aspect_default: fas::aspect< fas::type_list_n<
  connection_aspect< aspect_connection >,
  server_aspect< aspect_server >,
  // connection_manager< connection_manager_impl >,
  connection_base< fas::aspect_class >,
  connection< basic::tcp_connection >
>::type> 
{
};


template<typename A>
struct server_traits
{
  typedef typename fas::merge_aspect< A, aspect_default>::type aspect_type;
  typedef typename fas::aspect_class<aspect_type>::aspect aspect;
  
  ///
  /// connection
  ///
  typedef typename aspect::template advice_cast<_connection_aspect_>::type connection_aspect_type;
  
  template<typename ConnAspect>
  using connection_base_t = typename aspect::template advice_cast<_connection_base_>::type::template type<ConnAspect>;
  
  template<typename ConnAspect, template<typename> class ConnBase >
  using connection_t = typename aspect::template advice_cast<_connection_>::type::template type<ConnAspect, ConnBase>;
  
  typedef connection_t<connection_aspect_type, connection_base_t> connection_type;
  typedef typename connection_type::aspect::template advice_cast<_context_>::type connection_context_type;
  
  ///
  /// server
  ///
  typedef typename aspect::template advice_cast<_server_aspect_>::type server_aspect_type;

  typedef fas::aspect_class<server_aspect_type> server_base;
  typedef typename server_base::aspect::template advice_cast<_context_>::type server_context_type;

};

  
template<typename A = fas::aspect<> >
class server:
  public server_traits<A>::server_base
{
  typedef server_traits<A> traits;
  
public:
  typedef typename traits::server_base super;
  typedef typename traits::server_context_type server_context_type;
  
  typedef typename traits::connection_type connection_type;
  typedef typename traits::connection_context_type connection_context_type;
  
public:
  
  typedef typename super::aspect aspect;
  
  server( ::boost::asio::io_service& io_service)
    : _io_service(io_service)
  {
  }
  
  void start()
  {
    this->get_aspect().template getg<_initialize_>()(*this, std::ref(_io_service) );
    this->get_aspect().template get<_start_>()(*this);
  }
  
  void stop()
  {
    this->get_aspect().template get<_stop_>()(*this);
  }
  
  server_context_type& server_context() { return this->get_aspect().template get<_context_>();}
  const server_context_type& server_context() const { return this->get_aspect().template get<_context_>();}

  
  connection_context_type& connection_context() { return _connection_context;}
  const connection_context_type& connection_context() const { return _connection_context;}
  
private:
  ::boost::asio::io_service& _io_service;
  connection_context_type _connection_context;
};
  
template<typename Conn=echo_connection<> >
class tcp_server
{
public:
  // typedef TODO: Conn::rebind<>::type
  
  typedef Conn connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  //typedef typename connection_type::origin_connection origin_connection;
  typedef std::shared_ptr<connection_type> iconnection_ptr;
  typedef boost::asio::ip::tcp::socket socket_type;
  //typedef std::unique_ptr<socket_type> socket_ptr;
  typedef socket_type* socket_ptr;
  
  typedef typename connection_type::context_type context_type;
  
public:
  tcp_server( ::boost::asio::io_service& io_service)
    : _io_service(io_service)
    , _acceptor(io_service)
    // , _socket(nullptr)
    , _socket(io_service)

  {
    
  }
  
  /*
  connection_type& prototype() { return _prototype; }

  const connection_type& prototype() const { return _prototype; }
  */
  context_type& context()
  {
    return _context;
  }

  const context_type& context() const
  {
    return _context;
  }

  void start( const server_config& conf)
  {
    std::cout << conf.host << ":" << conf.port << std::endl;
    boost::asio::ip::tcp::resolver resolver(_io_service);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({conf.host, conf.port});
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
    this->do_accept();
    if ( !_acceptor.is_open() )
      std::cout << "dont open!" << std::endl;
  }

  void stop()
  {
    //std::cout << "server::stop _acceptor.close();" << std::endl;
    _acceptor.close();
    //std::cout << "server::stop c->close();" << std::endl;
    //std::vector<connection_ptr> all_conn
    for( auto& c : _connections )
      c->close();
      

    while (!_connections.empty())
    {
      _io_service.poll_one();
      std::cout << "_io_service.poll_one()" << std::endl;
    }
    //std::cout << "server::stop _connections.clear();" << std::endl;
    //_connections.clear();
    //std::cout << "server::stop done!" << std::endl;
    
  }

  void do_accept()
  {
    std::cout << "do_accept()!" << std::endl;
    //_socket = new socket_type(_io_service);
    _acceptor.async_accept(
      _socket,
      [this](boost::system::error_code ec)
      {
        
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!_acceptor.is_open())
        {
          std::cout << "!_acceptor.is_open()" << std::endl;
          //delete _socket;
          return;
        }

        std::cout << "accepted!" << std::endl;

        if (!ec)
        {
          connection_ptr pconn = std::make_shared<connection_type>();
          pconn->context() = _context;
          //connection_ptr pconn( _prototype.clone() );
          /*iconnection_ptr iconn;
          iconn = pconn;*/
          this->_connections.insert(pconn);
          pconn->start( std::move( this->_socket), [this](connection_ptr conn)->void{
            std::cout << "erase connection" << std::endl;
            this->_connections.erase(conn);
            std::cout << "erased!!!" << std::endl;
          });
          /*
          connection_manager_.start(std::make_shared<connection>(
              std::move(socket_), connection_manager_, request_handler_));
          */
        }

        this->do_accept();
      } // callback
    ); // async_accept
  }
  
private:
  ::boost::asio::io_service& _io_service;
  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor _acceptor;
  /// The next socket to be accepted.
  //socket_ptr _socket;
  socket_type _socket;
  std::set<connection_ptr> _connections;
  context_type _context;

};

}}