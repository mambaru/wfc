#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/tcp/rn/client_aspect.hpp>
#include <wfc/io/connection/connection.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 

template<typename T>
inline typename T::options_type update_options(T* self, typename T::options_type conf)
{
    auto startup = conf.connection.startup_handler;
    conf.connection.startup_handler = [self, startup]( ::wfc::io::io_id_t id, ::wfc::io::outgoing_handler_t clb, ::wfc::io::add_shutdown_handler_t add)
    {
      auto add_handler = [self](::wfc::io::io_id_t id) 
      {
        DAEMON_LOG_WARNING("Connection " << id << " closed. Reconnect...")
        
        self->post( [self](){
          self->connect();
        } );
      };
      
      add(add_handler);
      startup( id, clb, add);
    };
    return conf;

  return conf;
}
  
template<typename A = fas::aspect<> >
class client_base
  : public ::wfc::io::basic_io< typename fas::merge_aspect<A, client_aspect>::type >
  , public std::enable_shared_from_this< client_base<A> >
{
public:
  typedef client_base<A> self;
  typedef ::wfc::io::basic_io< typename fas::merge_aspect<A, client_aspect>::type > super;
  typedef typename super::options_type options_type;
  typedef typename super::aspect::template advice_cast<_connection_type_>::type connection_type; 
  typedef typename connection_type::descriptor_type socket_type;
  typedef std::shared_ptr<socket_type> socket_ptr;

  typedef boost::asio::ip::tcp::endpoint endpoint_type;
  
  typedef boost::asio::deadline_timer reconnect_timer;
  
  typedef ::wfc::asio::io_service io_service_type;
  reconnect_timer _reconnect_timer;
  std::shared_ptr<connection_type> _connection;
  
  client_base(io_service_type& io, const options_type& conf)
    : super( io, update_options(this, conf))
    , _reconnect_timer(io)
  {
    super::create(*this);
    if ( conf.connection.incoming_handler ==nullptr )
      abort();
  }
  
  void stop(std::function<void()> finalize)
  {
    super::stop(*this, finalize);
  }
  
  void connect()
  {
    COMMON_LOG_MESSAGE( "Client connect to " << this->options().host << ":" << this->options().port << " ..." )

    boost::asio::ip::tcp::resolver resolver( this->get_io_service() );
    boost::asio::ip::tcp::endpoint ep = *resolver.resolve({
      this->options().host, 
      this->options().port
    });

    auto psock = std::make_shared<socket_type>( this->get_io_service() );
    auto pthis = this->shared_from_this();
      
    psock->async_connect(ep, [ep, psock, pthis](const boost::system::error_code& ec)
    {
      if ( !ec )
      {
        COMMON_LOG_MESSAGE( "Client " << pthis->options().host << ":" << pthis->options().port << " connected!" )
        auto opt = pthis->options().connection;

        if ( opt.incoming_handler == nullptr )
          abort();

        pthis->_connection = std::make_shared<connection_type>( std::move(*psock), opt);
        pthis->_connection->start();
      }
      else
      {
        DAEMON_LOG_WARNING( pthis->options().host << ":" << pthis->options().port << ": " 
                            << ec.message() << " " << pthis->options().reconnect_timeout << " seconds to reconnect." )
          
        pthis->_reconnect_timer.expires_from_now( boost::posix_time::seconds( pthis->options().reconnect_timeout) );
        pthis->_reconnect_timer.async_wait([pthis, ep, psock](const boost::system::error_code& ec) 
        {
          if ( ec == boost::asio::error::operation_aborted )
            return;
          pthis->connect();
        });
      }
    });
  }

  void start()
  {
    super::start(*this);
    this->_reconnect_timer.expires_from_now( boost::posix_time::seconds( this->options().reconnect_timeout) );
    this->_reconnect_timer.async_wait( [this](const boost::system::error_code& ec) 
    {
      if ( ec == boost::asio::error::operation_aborted )
        return;
      this->connect();
    });
  }
  
};
  
}}}}}
