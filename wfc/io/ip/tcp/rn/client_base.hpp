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
  , public std::enable_shared_from_this< client_base<A> >
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
  wfc::io::incoming_handler_t _handler;

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
  
  client_base(wfc::io_service& io, const options_type& conf/*, ::wfc::io::incoming_handler handler = nullptr*/)
    : super( io, update_options(this, conf))
    , _reconnect_timer(io)
    , _handler( conf.incoming_handler ) // TODO: убрать 
  {
    super::create(*this);
  }
  
  void stop(std::function<void()> finalize)
  {
    super::stop(*this, finalize);
  }
  
  void connect()
  {
    TRACE_LOG_MESSAGE("void connect()...")
    //super::get_io_service().reset();
    /*super::get_io_service().post([this]()
    {
      TRACE_LOG_MESSAGE("void connect() posted")
      */
      boost::asio::ip::tcp::resolver resolver( this->get_io_service() );
      boost::asio::ip::tcp::endpoint ep = *resolver.resolve({
        this->options().host, 
        this->options().port
      });

      auto psock = std::make_shared<socket_type>( this->get_io_service() );
      std::weak_ptr<self> wself = this->shared_from_this();
      
      psock->get_io_service().reset();
      psock->async_connect(ep, /*super::strand().wrap(*/ [ep, psock, wself](const boost::system::error_code& ec)
      {
        auto pthis = wself.lock();
        if ( pthis == nullptr )
        {
          TRACE_LOG_MESSAGE("void connect() wself==nullptr")
          return;
        }
        
        if ( !ec )
        {
          COMMON_LOG_MESSAGE( "Client " << pthis->options().host << ":" << pthis->options().port << " connected!" )
          // TODO: для connection отдельный handler
          
          auto opt = pthis->options().connection;
          //opt.incoming_handler = this->_handler;
          pthis->_connection = std::make_shared<connection_type>( std::move(*psock), opt);
          pthis->_connection->start();
        }
        else
        {
          DAEMON_LOG_WARNING( pthis->options().host << ":" << pthis->options().port << ": " 
                              << ec.message() << " " << pthis->options().reconnect_timeout << " seconds to reconnect." )
          
          pthis->_reconnect_timer.expires_from_now( boost::posix_time::seconds( pthis->options().reconnect_timeout) );
          pthis->_reconnect_timer.async_wait([pthis, ep, psock](const boost::system::error_code& ) 
          {
            pthis->connect();
          });
        }
      }/*)*/);
    //});
  }

  void start()
  {
    super::start(*this);
    this->_reconnect_timer.expires_from_now( boost::posix_time::seconds( this->options().reconnect_timeout) );
    this->_reconnect_timer.async_wait( [this](const boost::system::error_code& ) {this->connect();} );
  }
  
};
  
}}}}}
