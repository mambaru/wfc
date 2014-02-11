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
class dgram_connection 
  : public connection_helper<A, AspectClass>::base_class
  , public std::enable_shared_from_this< dgram_connection<A, AspectClass> >
  , public iconnection
{
public:
  typedef typename connection_helper<A, AspectClass>::base_class super;
  typedef std::enable_shared_from_this< dgram_connection<A, AspectClass> > super_shared;
  
  typedef dgram_connection<A, AspectClass> self;
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

  ~dgram_connection()
  {
    std::cout << "~udp_connection_base() " << (_socket == nullptr) << std::endl;
    if ( _socket->is_open() )
      _socket->close();
    std::cout << "~udp_connection_base()" << std::endl;
  }
 
  dgram_connection(socket_ptr socket, release_function release)
    : _context()
    , _socket( socket )
    , _remote_endpoint(socket->remote_endpoint() )
    , _release(release)
    , _closed(true)
  {
    _strand = std::make_unique<strand_type>(_socket->get_io_service());
  }
 
  dgram_connection(socket_ptr socket, endpoint_type remote_endpoint, release_function release)
    : _context()
    , _socket( socket )
    , _remote_endpoint(remote_endpoint)
    , _release(release)
    , _closed(true)
  {
    _strand = std::make_unique<strand_type>(_socket->get_io_service());
  }
  
  dgram_connection(const dgram_connection&) = delete;
  dgram_connection& operator=(const dgram_connection&) = delete;
  
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

  /*
  socket_ptr socket() const
  {
    return _socket;
  }
  */
  
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
    
    std::cout << "get_io_service() { " << std::endl;
    io_service& io =  _socket->get_io_service();
    std::cout << "} get_io_service() " << std::endl;
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
    this->get_aspect().template get<_on_read_>()(*this, std::move(d) );
    if ( auto a = this->context().activity.lock() )
      a->update(this->shared_from_this());
  }

  virtual void close()
  {
    /*
    {
      std::lock_guard<mutex_type> lk( this->mutex() );
      if ( _closed )
        return;
      _closed = true;
    }*/
    
    _socket->get_io_service().post([this]
    { 
      std::lock_guard<mutex_type> lk( this->mutex() );
      std::cout << "POST CLOSE " << ( this->_socket == nullptr ) << std::endl;
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
    std::cout << "basic_connection::shutdown() {" << std::endl;
    this->get_aspect().template get<_shutdown_>()(*this);
    std::cout << "} basic_connection::shutdown()" << std::endl;
    //this->close();
  }

  void __release()
  {
    std::cout << " void __release() 1" << std::endl;
    if ( this->_release!=nullptr)
    {
      std::cout << " void __release() 2" << std::endl;
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
