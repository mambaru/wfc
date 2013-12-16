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

template<typename Conn>
struct connection_manager_impl
{
  typedef Conn connection_type;
public:
  void insert(std::shared_ptr<connection_type> conn)
  {
    _connections.insert(conn);
  }

  void erase(std::shared_ptr<connection_type> conn)
  {
    _connections.insert(conn);
  }
  
private:
  std::set< std::shared_ptr<connection_type> > _connections;
};

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
};
struct context_connection
{
  bool enable_stat;
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

  /*
  std::unique_ptr<acceptor> create()
  {
    std::unique_ptr<acceptor> acc = std::make_unique<acceptor>(_io_service);
    acc->_socket = std::make_unique<socket_type>(_io_service);
    acc->_acceptor = std::make_unique<acceptor_type>(_io_service);
    return acc;
  }
  */
  
  template<typename T>
  void start(T& t, acceptor_type* acceptor)
  {
    _acceptor = acceptor;
    typedef typename T::connection_type connection_type;
    _thread = std::thread([this, &t](){
      for(;;)
      {
        std::cout << "thread start " << std::this_thread::get_id() << std::endl;
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
          // Connection creator _new_connection_
          std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
          pconn->context() = t.connection_context();
          std::shared_ptr<iconnection> iconn = pconn;
          this->_connections.insert(pconn);
          
          std::cout << "connection start " << std::endl;
          // TODO: безопасный callback (aka callback_owner)
          pconn->start( std::move( this->_socket ), [this](std::shared_ptr<connection_type> pconn)->void{
            std::cout << "connection close " << std::endl;
            this->_connections.erase(pconn);
          });
        }
        else
          std::cout << "thread accept error: " << ec.message() << std::endl;
          
      }
    });
    sleep(1);
  }

  /*
  template<typename T>
  void do_accept(T& t)
  {
    this->_acceptor.async_accept(
      this->_socket,
      [this, &t](boost::system::error_code ec)
      {
        typedef typename T::connection_type connection_type;
        
        if (!_acceptor.is_open())
        {
          return;
        }

        if (!ec)
        {
          std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
          pconn->context() = t.connection_context();
          std::shared_ptr<iconnection> iconn = pconn;
          this->_connections.insert(pconn);
          
          // TODO: безопасный callback (aka callback_owner)
          pconn->start( std::move( this->_socket ), [this](std::shared_ptr<connection_type> pconn)->void{
            this->_connections.erase(pconn);
          });
        }
        this->do_accept(t);
      } // callback
    ); // async_accept
  }
  */

private:  
  ::boost::asio::io_service::work _work;
  socket_type _socket;
  acceptor_type* _acceptor;
  //::boost::asio::io_service& _io_service;
  /*
  std::unique_ptr<socket_type> _socket;
  std::unique_ptr< acceptor_type > _acceptor;
  */
  std::set< std::shared_ptr<iconnection> > _connections;
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
    //: _accept_thread(io)
  {
    _io_service = &io;
    _acceptor = std::make_unique<acceptor_type>(io);
    /* initialize */
    /*
    _socket = std::make_unique<socket_type>(io);
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
    
    _accept_thread = std::make_unique<accept_thread>(*_io_service);
    _accept_thread2 = std::make_unique<accept_thread>(*_io_service);
    std::cout << "start..." << std::endl;
    _accept_thread->start(t, &(*_acceptor) );
    _accept_thread2->start(t, &(*_acceptor) );
    std::cout << "start... ok" << std::endl;
    

    /*
    _acceptor2->assign(endpoint.protocol(), _acceptor->native() );
    
    _acceptor2->open(endpoint.protocol());
    _acceptor2->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor2->bind(endpoint);
    _acceptor2->listen();
    
    this->do_accept(t, *_acceptor2, true);
    this->do_accept(t, *_acceptor, false);
    */
    //_acceptor->close();
  }
  
  template<typename T>
  void do_accept(T& t, acceptor_type& acceptor, bool flags)
  {
    acceptor.async_accept(
      *_socket,
      [this, &t, &acceptor, flags](boost::system::error_code ec)
      {
        std::cout << "accept " << flags << std::endl;
        /*
        
        if ( flags )
        {
          std::cout << "sleep " << std::endl;
          sleep(10000000);
        }
        std::cout << "ready " << std::endl;
        */
        typedef typename T::connection_type connection_type;
        
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor.is_open())
        {
          return;
        }

        if (!ec)
        {
          std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
          pconn->context() = t.connection_context();
          std::shared_ptr<iconnection> iconn = pconn;
          this->_connections.insert(pconn);
          
          // TODO: безопасный callback (aka callback_owner)
          pconn->start( std::move( *(this->_socket) ), [this](std::shared_ptr<connection_type> pconn)->void{
            this->_connections.erase(pconn);
          });
        }
        this->do_accept(t, acceptor, flags);
        
      } // callback
    ); // async_accept
  }

  
private:
  std::unique_ptr<socket_type> _socket;
  ::boost::asio::io_service* _io_service;
  std::unique_ptr<accept_thread> _accept_thread;
  std::unique_ptr<accept_thread> _accept_thread2;
  
  std::unique_ptr< acceptor_type > _acceptor;
  
  /*
  std::unique_ptr< acceptor_type > _acceptor2;
  */
  
  std::set< std::shared_ptr<iconnection> > _connections;
};


struct aspect_server: fas::aspect< fas::type_list_n<
  context<context_server>,
  fas::advice<_start_, ad_acceptor>,
  fas::group<_initialize_,  _start_>
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
  connection_manager< connection_manager_impl >,
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