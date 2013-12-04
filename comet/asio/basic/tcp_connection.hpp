#pragma once
#include <fas/aop.hpp>
#include <fas/type_list.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <list>
#include <comet/callback/callback_owner.hpp>
#include <comet/asio/iconnection.hpp>
#include <comet/asio/basic/tags.hpp>
#include <comet/asio/basic/aspect.hpp>

namespace mamba{ namespace comet{ namespace inet{ namespace basic{

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class tcp_connection_base
  : public AspectClass<A>
  , public std::enable_shared_from_this< tcp_connection_base<A, AspectClass> >
  , public iconnection
{
  typedef AspectClass<A> super;
  typedef tcp_connection_base<A, AspectClass> self;
  typedef std::shared_ptr<iconnection> self_ptr;
  typedef std::function<void( self_ptr )> release_function;

  typedef boost::asio::ip::tcp::socket socket_type;
  typedef std::unique_ptr<socket_type> socket_ptr;

  typedef boost::asio::strand strand_type;
  typedef std::unique_ptr<strand_type> strand_ptr;

public:

  ~tcp_connection_base()
  {
    std::cout << "~tcp_connection_base()" << std::endl;
  }
  
  tcp_connection_base()
  {
    
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

  void start(socket_type socket, std::function<void( std::shared_ptr<iconnection> )> release)
  {
    //_write_counter = 0;
    _release = release;
    
    //_socket = socket_ptr(socket);
    _socket = socket_ptr( new socket_type(std::move(socket)));
    _strand = strand_ptr( new strand_type(_socket->get_io_service()));

    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    _socket->io_control(non_blocking_io);


    std::cout << "start..." << std::endl;
    this->get_aspect().template get<_input_>()(*this);
    std::cout << "...start" << std::endl;

    //this->do_read();
    /*
    for (int i =0; i < 100; ++i)
    {
      std::stringstream ss;
      for (int k =0; k < 100; ++k)
        ss << "Привет чувак "<< i<<"-" << k <<",";
      ss << "\r\n";
      std::string *str = new std::string(ss.str());
      this->write(str->c_str(), str->size() );
    }*/
    //this->do_write();
    //_socket = std::move(socket);
  }
  
  virtual void close()
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
    
    /*
    _socket->async_read_some(
      boost::asio::buffer(_buffer),
      [this](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          this->get_aspect().template get<_on_read_>()
          (*this, _buffer.data(), bytes_transferred );
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
          release();
            // TODO: stop!
        }
      }
    );*/
  }

  void write(const char* data, size_t size)
  {
    typedef typename super::aspect::template advice_cast<_outgoing_>::type::data_type data_type;
    typedef typename super::aspect::template advice_cast<_outgoing_>::type::data_ptr data_ptr;
    this->get_aspect().template get<_outgoing_>()(*this, data_ptr( new data_type(data, data+size)));
  }

  /*
  size_t _write_counter;
  
  size_t write_some(const char* data, size_t size)
  {
    if ( _write_counter!=0 )
      return 0;

    return _socket->write_some( boost::asio::buffer(data, size) );
  }
  */
  
  /*
  void do_write()
  {
    ++_write_counter;
    std::cout << "do_write _write_counter==" << _write_counter << std::endl;
    //std::string str("Привет чувак\r\n");
    _socket->async_write_some(
      this->get_aspect().template get<_write_>().to_buffers(),
      //boost::asio::buffer(str.c_str(), str.size()),
      [this](boost::system::error_code ec, std::size_t s){
        this->_write_counter-=1;
        std::cout << "writed " << s << " _write_counter==" << this->_write_counter << std::endl;
        this->get_aspect().template get<_write_>().transferred(*this, s);
     }
    );
  }
  */
  
private:
  release_function _release;
  socket_ptr _socket;
  strand_ptr _strand;
  callback_owner _owner;
  //socket_type _socket;
  /// Buffer for incoming data.
  std::array<char, 8192*2> _buffer;
};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class tcp_connection:
  public tcp_connection_base< typename fas::merge_aspect<A, aspect>::type, AspectClass>
{

};


}}}}
