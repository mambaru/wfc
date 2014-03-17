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
  
  wfc::io::handler _handler;
  // TODO: сделать массив
  std::shared_ptr<connection_type> _connection;
  
  client_base(wfc::io_service& io, const options_type& conf, ::wfc::io::handler handler = nullptr)
    : super( io, conf)
    , _handler( handler )
  {
    super::create(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }

  void start()
  {
    self& t = *this;
    super::start(t);
    
    t.dispatch([&t]()
    {
      typedef typename connection_type::descriptor_type descriptor_type;
      descriptor_type desc( t.get_io_service() );
  
      boost::asio::ip::tcp::resolver resolver( t.get_io_service() );
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
        t.options().host, 
        t.options().port
      });
      
      
      typedef typename connection_type::descriptor_type socket_type;
      auto sock = std::make_shared<socket_type>( t.get_io_service() );
      sock->async_connect(endpoint, [sock, &t](const boost::system::error_code& ec)
      {
        if ( !ec )
        {
          // TODO: для connection отдельный handler
          t._connection = std::make_shared<connection_type>( std::move(*sock), t.options().connection, t._handler);
          t._connection->start();
        }
        else
        {
          abort();
        }
      });
    });
  }
  
};
  
}}}}}
