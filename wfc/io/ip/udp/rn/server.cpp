

#include <wfc/io/ip/udp/rn/server.hpp>
#include <wfc/io/ip/udp/rn/server_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

server::~server()
{
}

  
server::server(wfc::io_service& io, const server::options_type& conf)
  : _impl( std::make_shared<server_impl>(io, conf) )
{
  
}

void server::start()
{
  DEBUG_LOG_MESSAGE( "##################################### void server::start()" )
  _impl->start();
}

void server::stop()
{
  DEBUG_LOG_MESSAGE( "UDP server::stop() -1-" )
  if ( _impl != nullptr )
  {
    DEBUG_LOG_MESSAGE( "UDP server::stop() -2-" )
    _impl->stop(nullptr);
    _impl.reset();
  }
}
  
void server::shutdown()
{
}

}}}}}
