#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/tcp/rn/client_aspect.hpp>
#include <wfc/io/connection/connection.hpp>
#include <wfc/io/types.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 

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
  
  options_type init_options(options_type conf)
  {
    auto startup = conf.connection.startup_handler;
    conf.connection.startup_handler = [this, startup]( ::wfc::io::io_id_t id, ::wfc::io::callback clb, ::wfc::io::add_shutdown_handler add)
    {
      add([this](::wfc::io::io_id_t id) {
        DAEMON_LOG_WARNING("Connection " << id << " closed. Reconnect...")
        this->connect();
      });
      startup( id, clb, add);
    };
    return conf;
  }
  
  client_base(wfc::io_service& io, const options_type& conf, ::wfc::io::handler handler = nullptr)
    : super( io, init_options(conf))
    , _reconnect_timer(io)
    , _handler( handler )
  {
    super::create(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }
  
  void connect(/*endpoint_type ep, socket_ptr psock*/)
  {
        boost::asio::ip::tcp::resolver resolver( this->get_io_service() );
    boost::asio::ip::tcp::endpoint ep = *resolver.resolve({
      this->options().host, 
      this->options().port
    });

    auto psock = std::make_shared<socket_type>( this->get_io_service() );

      psock->async_connect(ep, [ep, psock, this](const boost::system::error_code& ec)
      {
        if ( !ec )
        {
          DAEMON_LOG_MESSAGE( "Connected!" )
          // TODO: для connection отдельный handler
          this->_connection = std::make_shared<connection_type>( std::move(*psock), this->options().connection, this->_handler);
          this->_connection->start();
        }
        else
        {
          DAEMON_LOG_WARNING( ec.message() << " " << this->options().reconnect_timeout << " seconds to reconnect." )
          _reconnect_timer.expires_from_now( boost::posix_time::seconds( this->options().reconnect_timeout) );
          _reconnect_timer.async_wait([this, ep, psock](const boost::system::error_code& ) {
            this->connect(/*ep, psock*/);
          });
          /*
            _idle_timer->expires_at(_idle_timer->expires_at() + boost::posix_time::milliseconds(_conf.idle_timeout_ms));
            _idle_timer->async_wait([this](const boost::system::error_code& e)
            {
              this->_idle();  
            });*/
          //abort();
        }
      });
    
  }

  void start()
  {
    super::start(*this);
    this->connect();
  /*
    boost::asio::ip::tcp::resolver resolver( this->get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      this->options().host, 
      this->options().port
    });

    auto sock = std::make_shared<socket_type>( this->get_io_service() );
    this->connect( endpoint, sock);
  */
    
    /*
    this->dispatch([this]()
    {
      typedef typename connection_type::descriptor_type descriptor_type;
      descriptor_type desc( this->get_io_service() );
  
      boost::asio::ip::tcp::resolver resolver( this->get_io_service() );
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
        this->options().host, 
        this->options().port
      });
      
      
      auto sock = std::make_shared<socket_type>( this->get_io_service() );
      sock->async_connect(endpoint, [sock, this](const boost::system::error_code& ec)
      {
        if ( !ec )
        {
          // TODO: для connection отдельный handler
          this->_connection = std::make_shared<connection_type>( std::move(*sock), this->options().connection, this->_handler);
          this->_connection->start();
        }
        else
        {
          abort();
        }
      });
    });*/
  }
  
};
  
}}}}}
