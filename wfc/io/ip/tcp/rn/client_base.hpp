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
    conf.connection.startup_handler = [self, startup]( ::wfc::io::io_id_t id, ::wfc::io::callback clb, ::wfc::io::add_shutdown_handler add)
    {
      auto add_handler = [self](::wfc::io::io_id_t id) 
      {
        DAEMON_LOG_WARNING("Connection " << id << " closed. Reconnect...")
        //std::bind( &self::connect, self)
        self->post( [self](){
          self->connect();
        } );
        //self->post( std::bind( &self::connect, self) );
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
  //public wfc::io::connection::connection< typename fas::merge_aspect<A, client_aspect>::type >
{
  //typedef wfc::io::connection::connection< typename fas::merge_aspect<A, client_aspect>::type > super;
public:
  typedef client_base<A> self;
  typedef ::wfc::io::basic_io< typename fas::merge_aspect<A, client_aspect>::type > super;
  typedef typename super::options_type options_type;
  typedef typename super::aspect::template advice_cast<_connection_type_>::type connection_type; 
  //typedef typename super::descriptor_type descriptor_type;
  typedef typename connection_type::descriptor_type socket_type;
  typedef std::shared_ptr<socket_type> socket_ptr;

  typedef boost::asio::ip::tcp::endpoint endpoint_type;
  
  typedef boost::asio::deadline_timer reconnect_timer;
  reconnect_timer _reconnect_timer;
  wfc::io::handler _handler;

  // TODO: сделать массив
  std::shared_ptr<connection_type> _connection;
  
  
  /*
  options_type init_options(options_type conf)
  {
    auto startup = conf.connection.startup_handler;
    conf.connection.startup_handler = [this, startup]( ::wfc::io::io_id_t id, ::wfc::io::callback clb, ::wfc::io::add_shutdown_handler add)
    {
      auto add_handler = [this](::wfc::io::io_id_t id) 
      {
        DAEMON_LOG_WARNING("Connection " << id << " closed. Reconnect...")
        this->post( std::bind( &self::connect, this) );
      };
      
      add(add_handler);
      startup( id, clb, add);
    };
    return conf;
  }
  */
  
  client_base(wfc::io_service& io, const options_type& conf, ::wfc::io::handler handler = nullptr)
    : super( io, update_options(this, conf))
    , _reconnect_timer(io)
    , _handler( handler )
  {
    super::create(*this);
  }
  
  void stop(std::function<void()> finalize)
  {
    super::stop(*this, finalize);
  }
  
  void connect()
  {
    TRACE_LOG_MESSAGE("void connect()")
    boost::asio::ip::tcp::resolver resolver( this->get_io_service() );
    boost::asio::ip::tcp::endpoint ep = *resolver.resolve({
      this->options().host, 
      this->options().port
    });

    auto psock = std::make_shared<socket_type>( this->get_io_service() );

    psock->async_connect(ep, super::strand().wrap( [ep, psock, this](const boost::system::error_code& ec)
    {
      if ( !ec )
      {
        COMMON_LOG_MESSAGE( "Client " << this->options().host << ":" << this->options().port << " connected!" )
          // TODO: для connection отдельный handler
        this->_connection = std::make_shared<connection_type>( std::move(*psock), this->options().connection, this->_handler);
        this->_connection->start();
      }
      else
      {
        DAEMON_LOG_WARNING( this->options().host << ":" << this->options().port << ": " 
                            << ec.message() << " " << this->options().reconnect_timeout << " seconds to reconnect." )
        
        this->_reconnect_timer.expires_from_now( boost::posix_time::seconds( this->options().reconnect_timeout) );
        this->_reconnect_timer.async_wait([this, ep, psock](const boost::system::error_code& ) 
        {
           this->connect();
        });
      }
    }));
  }

  void start()
  {
    super::start(*this);
    
    super::dispatch([this]()
    {
      this->connect();
    });
  }
  
};
  
}}}}}
