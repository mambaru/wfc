#pragma once

#include <wfc/io/connection/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>

namespace wfc{ namespace io{ namespace connection{
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class connection
  : public descriptor_holder<A, AspectClass>
  , public std::enable_shared_from_this< connection<A, AspectClass> >
{
public:
  
  typedef connection<A, AspectClass> self;
  typedef descriptor_holder<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::options_type options_type;
  
public:
  
  ~connection()
  {
    
  }

  connection(const connection& ) = delete;
  void operator = (const connection& conf) = delete;

  
  connection(descriptor_type&& desc, const options_type& opt/*, wfc::io::incoming_handler handler = nullptr*/)
    : super( std::move(desc), opt/*, handler*/)
  {
    super::create(*this);
    
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    super::descriptor().io_control(non_blocking_io);

    boost::asio::socket_base::keep_alive option(opt.keep_alive);
    super::descriptor().set_option(option);

    // boost::asio::socket_base::linger option(true, 30);
    // socket.set_option(option);

    // boost::asio::socket_base::receive_buffer_size option(8192);
    // socket.set_option(option);
    
    // boost::asio::socket_base::send_buffer_size option(8192);
    // socket.set_option(option);

    // boost::asio::socket_base::linger option(true, 30);
    // socket.set_option(option);
    
    /*
    //if ( setsockopt((int)d, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val)))
    //  throw socket_error( "fas::system::inet::tcp_nodelay: " );
    
    //if ( setsockopt((int)d, SOL_SOCKET, SO_KEEPALIVE, (const char*)&val, sizeof(val)))
    //  throw socket_error( "fas::system::inet::keepalive: " );
    
    boost::asio::ip::tcp::no_delay option(true);
    super::descriptor().set_option(option);

    boost::asio::socket_base::receive_buffer_size option(8192);
    socket.set_option(option);
    
    boost::asio::socket_base::keep_alive option(true);
    socket.set_option(option);

    boost::asio::socket_base::linger option(true, 30);
    socket.set_option(option);

     */ 

  }

  void start()
  {
    super::start(*this);
  }
  
  void stop(std::function<void()> finalize)
  {
    super::stop(*this, finalize);
  }
  
  bool status() const
  {
    return this->get_aspect().template get<_status_>();
  }
  
private:

};


}}}
