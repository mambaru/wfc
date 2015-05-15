

#include <wfc/io/ip/udp/rn/server.hpp>
#include <wfc/io/ip/udp/rn/server_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

server::~server()
{
}

  
server::server(io_service_type& io, const server::options_type& conf)
  : _impl( std::make_shared<server_impl>(io, conf) )
{
  
}

void server::start()
{
  _impl->start();
}

void server::stop()
{
  if ( _impl != nullptr )
  {
    _impl->stop(nullptr);
    _impl.reset();
  }
}
  
void server::shutdown()
{
}

}}}}}
