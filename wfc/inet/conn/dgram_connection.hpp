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
class dgram_connection final
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
  typedef socket_type* socket_ptr;

  typedef boost::asio::strand strand_type;
  typedef std::unique_ptr<strand_type> strand_ptr;

public:
  
  typedef typename super::aspect::template advice_cast<_context_>::type context_type;
  typedef callback_owner<> owner_type;

  ~dgram_connection()
  {
    //_socket->close();
    std::cout << "~udp_connection_base()" << std::endl;
  }
  
  dgram_connection(socket_type* socket, endpoint_type remote_endpoint, release_function release)
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
    return _socket->get_io_service();
  }
  
  strand_type& strand()
  {
    return *_strand;
  }
  
  const strand_type& strand() const
  {
    return *_strand;
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
    if ( !_closed )
      return;
    _closed = false;
    this->get_aspect().template getg<_startup_>()( *this, fas::tag<_startup_>() );
    this->get_aspect().template get<_start_>()(*this, fas::tag<_start_>() );
  }
  
  virtual void close()
  {
    if ( _closed )
      return;
    _closed = true;
    _socket->get_io_service().post([this]{ 
      if ( _closed )
        this->__release(); 
    });
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
  
  void shutdown()
  {
    // std::cout << "basic_connection shutdown()" << std::endl;
    //this->get_aspect().template get<_shutdown_>()(*this);
    this->close();
  }

  void __release()
  {
    if ( this->_release!=nullptr)
    {
      _release( super_shared::shared_from_this() );
    }
  }

  void write(const char* data, size_t size)
  {
    this->get_aspect().template get<_write_>()(*this, std::make_unique<data_type>(data, data+size));
  }

  template<typename T>
  void write(const T& data)
  {
    this->get_aspect().template get<_write_>()(*this, std::make_unique<data_type>(data.begin(), data.end()));
  }

  void write(const char* data)
  {
    this->get_aspect().template get<_write_>()(*this, std::make_unique<data_type>(data, data + std::strlen(data) ) );
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
