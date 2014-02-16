#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <wfc/inet/conn/helper.hpp>
#include <wfc/inet/types.hpp>
#include <wfc/io_service.hpp>

#include <wfc/memory.hpp>

namespace wfc{ namespace inet{ 
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class connection 
  : public conn::helper<A, AspectClass>::base_class
  , public std::enable_shared_from_this< connection <A, AspectClass> >
  //, public iconnection
{
public:
  typedef connection<A, AspectClass> self;
  
  typedef conn::helper<A, AspectClass> helper;
  typedef typename helper::base_class super;
  
  typedef std::enable_shared_from_this< self > super_shared;
  
  typedef self connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  // typedef std::function<void( connection_ptr )> release_function;

  typedef typename helper::socket_type socket_type;
  // typedef typename socket_type::endpoint_type endpoint_type;
  //typedef boost::asio::ip::tcp::socket socket_type;
  typedef std::shared_ptr<socket_type> socket_ptr;

  /*
  typedef boost::asio::strand strand_type;
  typedef std::unique_ptr<strand_type> strand_ptr;
  */

public:

  typedef typename helper::context_type context_type;
  //typedef typename super::aspect::template advice_cast<_context_>::type context_type;
  /*
  typedef callback_owner<> owner_type;
  typedef std::unique_ptr< owner_type > owner_ptr;
  */
  

  /*
  ~connection ()
  {
    this->close();
    */
    /*
    std::cout << "~connection ()" << std::endl; 
    if ( _socket!=nullptr && _socket->is_open() )
      _socket->close();
    */
  /*}*/

  /*
  connection ()
    //: _closed(true)
  {
  }
  */

 /*
  connection (socket_ptr socket, release_function release)
    : _context()
    , _socket( socket )
    , _remote_endpoint(socket->remote_endpoint() )
    , _release(release)
    , _closed(true)
  {
    _strand = std::make_unique<strand_type>(_socket->get_io_service());
  }
 
  connection (socket_ptr socket, endpoint_type remote_endpoint, release_function release)
    : _context()
    , _socket( socket )
    , _remote_endpoint(remote_endpoint)
    , _release(release)
    , _closed(true)
  {
    _strand = std::make_unique<strand_type>(_socket->get_io_service());
  }
  */
  
 
  connection () {};
  connection(const connection&) = delete;
  connection & operator=(const connection&) = delete;
 
  
  /*
   * Не привязывемся к сокету
  const endpoint_type& remote_endpoint() const
  {
    return _remote_endpoint;
  }
  */
  
  context_type& context()
  {
    // return _context;
    return this->get_aspect().template get<_context_>();
  }

  const context_type& context() const
  {
    //return _context;
    return this->get_aspect().template get<_context_>();
  }

  socket_type& socket()
  {
    return *(this->get_aspect().template get<_socket_ptr_>());
    //return *_socket;
  }
  
  const socket_type& socket() const
  {
    return *(this->get_aspect().template get<_socket_ptr_>());
    //return *_socket;
  }
  
  /*
  io_service& get_io_service()
  {
    io_service& io =  _socket->get_io_service();
    return io;
  }
  */
  
  
  /*
  strand_type& strand()
  {
    return *_strand;
  }
  
  const strand_type& strand() const
  {
    return *_strand;
  }
  */
  
  /*
  mutex_type& mutex() 
  {
    return _owner.mutex();
  }
  */
  
  /*

  owner_type& owner()
  {
    return _owner;
  }
  
  const owner_type& owner() const
  {
    return _owner;
  }
  
  */

  template<typename F>
  void dispatch(F f)
  {
    this->get_aspect().template get<_dispatch_>()( f );
    //this->_socket->get_io_service().dispatch(this->_strand->wrap( f ));
  }

  template<typename F>
  void post(F f)
  {
    this->get_aspect().template get<_post_>()( f );
    //this->_socket->get_io_service().dispatch(this->_strand->wrap( f ));
  }
  
  /*
  template<typename F>
  void strand_dispatch(F f)
  {
    this->_socket->get_io_service().dispatch(this->_strand->wrap( f ));
  }

  template<typename F>
  void strand_post(F f)
  {
    this->_socket->get_io_service().post(this->_strand->wrap( f ));
  }
  */
  
  template<typename C>
  void configure(const C& conf)
  {
    this->get_aspect().template getg<_configure_>()( *this, conf );
  }

  template<typename Init>
  void initialize(const Init& init)
  {
    this->get_aspect().template getg<_initialize_>()( *this, init );
  }

  /*
  void initialize(socket_ptr socket, release_function release_fun)
  {
    //!! Args&& ...args
    //!! this->get_aspect().template getg<_configure_>()( *this, std::forward<Args>(args)... );
    // Формируем структуру инициализации
    // this->get_aspect().template getg<_initialize_>()( *this, init );
    
    _socket = socket;
    _strand = std::make_unique<strand_type>(_socket->get_io_service());
    _owner = std::make_unique<owner_type>(); // owner оставляем, все остальное в _initialize_ ( или вообще убрать из api)
    //_remote_endpoint = socket->remote_endpoint();
    _release_function = release_fun;
    this->get_aspect().template getg<_initialize_>()( *this );
  }
  */
 
 /* TODO: для UDP другой способ придумать
  void initialize(socket_ptr socket, endpoint_type remote_endpoint, release_function release_fun)
  {
    _socket = socket;
    _strand = std::make_unique<strand_type>(_socket->get_io_service());
    _remote_endpoint = remote_endpoint;
    _release_function = release_fun;
    this->get_aspect().template getg<_initialize_>()( *this );
  }
  */

  connection_ptr clone()
  {
    connection_ptr pconn = std::make_shared<connection_type>();
    static_cast<super&>(*pconn) = static_cast<super&>(*this);
    //pconn->_context = this->_context;
    return pconn;
  }
  
  void start()
  {
    // TODO: start
    this->get_aspect().template get<_start_>()(*this);
    /*
    this->strand_dispatch([this]()
    {
      if ( !_closed )
        return;
      _closed = false;
      this->get_aspect().template get<_start_>()(*this);
    });
    */
  }
  
  void stop()
  {
    // TODO: close
    this->get_aspect().template get<_stop_>()(*this/*, _release_function*/);
    /*
    this->strand_post([this]()
    {
      if ( !_closed )
      {
        _closed = true;
        this->release(); 
      }
    });
    */
  }

  void shutdown()
  {
    this->get_aspect().template get<_shutdown_>()(*this);
  }

  /*
  void on_read(data_ptr d)
  {
    auto pd = std::make_shared<data_ptr>( std::move(d));
    this->strand_dispatch([this, pd]()
    {
      data_ptr d = std::move(*pd);
      this->get_aspect().template getg<conn::_on_read_>()(*this, d->begin(), d->end() );
      this->get_aspect().template get<conn::_incoming_>()(*this, std::move(d) );
    });
  }
  */

  /*
  void send(data_ptr d)
  {
    auto pd = std::make_shared<data_ptr>(std::move(d));
    this->strand_dispatch([this, pd]()
    {
      this->get_aspect().template get<conn::_write_>()(*this, std::move(*pd));
    });
  }
  */

private:
  
  /*
  void release()
  {
    if ( this->_release_function!=nullptr)
    {
      this->_release_function( super_shared::shared_from_this() );
    }
  }
  */

  
  // context_type _context;
  // socket_ptr _socket;
  // endpoint_type _remote_endpoint;
  // strand_ptr _strand;
  // release_function _release_function;
  // owner_ptr _owner;
  // bool _closed;
};

}}
