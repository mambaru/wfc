#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <wfc/callback/callback_owner.hpp>
#include <wfc/inet/iconnection.hpp>
#include <wfc/inet/basic/tags.hpp>
#include <wfc/inet/basic/aspect.hpp>

namespace wfc{ namespace inet{ namespace basic{

struct f_initialize
{
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    t.get_aspect().template get<Tg>().initialize(t);
  }
};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class tcp_connection_base final
  : public AspectClass<A>
  , public std::enable_shared_from_this< tcp_connection_base<A, AspectClass> >
  //, public iconnection
{
public:
  typedef AspectClass<A> super;
  typedef tcp_connection_base<A, AspectClass> self;
  typedef self connection_type;
  //typedef self origin_connection;
  typedef std::shared_ptr<connection_type> connection_ptr;
  typedef std::function<void( connection_ptr )> release_function;

  typedef boost::asio::ip::tcp::socket socket_type;
  typedef std::unique_ptr<socket_type> socket_ptr;

  typedef boost::asio::strand strand_type;
  typedef std::unique_ptr<strand_type> strand_ptr;

public:
  
  typedef typename super::aspect::template advice_cast<_context_>::type context_type;

  ~tcp_connection_base()
  {
    std::cout << "~tcp_connection_base()" << std::endl;
  }
  
  tcp_connection_base()
    : _context()
  {
    fas::for_each_group<_initialize_>(*this, f_initialize());
  }
  
  /*
  connection_ptr clone() const
  {
    connection_ptr clon = std::make_shared<connection_type>();
    static_cast<super&>(*clon) = *this;
  }*/
  
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
  
  strand_type& strand()
  {
    return *_strand;
  }
  
  const strand_type& strand() const
  {
    return *_strand;
  }
  
  callback_owner& owner()
  {
    return _owner;
  }
  
  const callback_owner& owner() const
  {
    return _owner;
  }

  void start(socket_type socket, release_function release)
  {
    _release = release;
    _socket = socket_ptr( new socket_type(std::move(socket)));
    _strand = strand_ptr( new strand_type(_socket->get_io_service()));
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    _socket->io_control(non_blocking_io);
    std::cout << "start..." << std::endl;
    this->get_aspect().template get<_reader_>()(*this);
    std::cout << "...start" << std::endl;
  }
  
  void close()
  {
    _socket->close();
  }
  
  void shutdown()
  {
  }

  void release()
  {
    _release( std::enable_shared_from_this< tcp_connection_base<A, AspectClass> >::shared_from_this() );
  }

  void do_read()
  {
  }

  void write(const char* data, size_t size)
  {
    typedef typename super::aspect::template advice_cast<_outgoing_>::type::data_type data_type;
    typedef typename super::aspect::template advice_cast<_outgoing_>::type::data_ptr data_ptr;
    this->get_aspect().template get<_outgoing_>()(*this, data_ptr( new data_type(data, data+size)));
  }

  
private:
  context_type _context;
  release_function _release;
  socket_ptr _socket;
  strand_ptr _strand;
  callback_owner _owner;
  std::array<char, 8192*2> _buffer;
};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
using tcp_connection = tcp_connection_base< typename fas::merge_aspect<A, aspect>::type, AspectClass>;


}}}
