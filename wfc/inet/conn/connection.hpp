#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <wfc/inet/conn/iconnection.hpp>
#include <wfc/inet/conn/connection_helper.hpp>
#include <wfc/inet/types.hpp>
#include <wfc/io_service.hpp>

namespace wfc{ namespace inet{ 
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class connection 
  : public connection_helper<A, AspectClass>::base_class
  , public std::enable_shared_from_this< connection <A, AspectClass> >
  //, public iconnection
{
public:
  typedef typename connection_helper<A, AspectClass>::base_class super;
  typedef std::enable_shared_from_this< connection <A, AspectClass> > super_shared;
  
  typedef connection <A, AspectClass> self;
  typedef self connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  typedef std::function<void( connection_ptr )> release_function;

  typedef typename super::aspect::template advice_cast<_socket_type_>::type socket_type;
  typedef typename socket_type::endpoint_type endpoint_type;
  //typedef boost::asio::ip::tcp::socket socket_type;
  typedef std::shared_ptr<socket_type> socket_ptr;

  typedef boost::asio::strand strand_type;
  typedef std::unique_ptr<strand_type> strand_ptr;

public:
  
  typedef typename super::aspect::template advice_cast<_context_>::type context_type;
  typedef callback_owner<> owner_type;
  typedef owner_type::mutex_type  mutex_type;

  ~connection ()
  {
    if ( _socket->is_open() )
      _socket->close();
  }
 
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
  
  connection(const connection&) = delete;
  connection & operator=(const connection&) = delete;
  
  const endpoint_type& remote_endpoint() const
  {
    return _remote_endpoint;
  }
  
  context_type& context()
  {
    return _context;
  }

  const context_type& context() const
  {
    return _context;
  }

  socket_type& socket()
  {
    return *_socket;
  }
  
  const socket_type& socket() const
  {
    return *_socket;
  }
  
  io_service& get_io_service()
  {
    io_service& io =  _socket->get_io_service();
    return io;
  }
  
  strand_type& strand()
  {
    return *_strand;
  }
  
  const strand_type& strand() const
  {
    return *_strand;
  }
  
  mutex_type& mutex() 
  {
    return _owner.mutex();
  }
  
  owner_type& owner()
  {
    return _owner;
  }
  
  const owner_type& owner() const
  {
    return _owner;
  }

  void start()
  {
    // TODO: dispatch
    {
      std::lock_guard<mutex_type> lk( this->mutex() );
      if ( !_closed )
        return;
      _closed = false;
    }
    
    this->get_aspect().template getg<_startup_>()( *this, fas::tag<_startup_>() );
    this->get_aspect().template get<_start_>()(*this, fas::tag<_start_>() );
  }
  
  
  virtual boost::asio::ip::address remote_address()
  {
    return _remote_endpoint.address();
  }
  
  virtual unsigned short remote_port()
  {
    return _remote_endpoint.port();
  }
  
  virtual void on_read(data_ptr d)
  {
    this->get_aspect().template getg<_on_read_>()(*this, d->begin(), d->end() );
    this->get_aspect().template get<_incoming_>()(*this, std::move(d) );
    /*
     *
    Отработка будет в одном из _on_read_
    if ( auto a = this->context().activity.lock() )
      a->update(this->shared_from_this());
    */
  }

  virtual void close()
  {
    // TODOD: strand
    _socket->get_io_service().post([this]
    { 
      std::lock_guard<mutex_type> lk( this->mutex() );
      if ( !_closed )
      {
        _closed = true;
        this->__release(); 
      }
      this->_socket = nullptr;
    });
  }

  void shutdown()
  {
    this->get_aspect().template get<_shutdown_>()(*this);
  }

  void __release()
  {
    if ( this->_release!=nullptr)
    {
      _release( super_shared::shared_from_this() );
    }
  }
  
  void send(data_ptr d)
  {
    this->get_aspect().template get<_write_>()(*this, std::move(d) );
  }

private:
  context_type _context;
  socket_ptr _socket;
  endpoint_type _remote_endpoint;
  strand_ptr _strand;
  release_function _release;
  owner_type _owner;
  std::atomic<bool> _closed;
};

}}
