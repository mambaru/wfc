

#include <wfc/io/ip/tcp/rn/server.hpp>
#include <wfc/io/ip/tcp/rn/server_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

server::~server()
{
  //_impl.reset();
}

  
server::server(wfc::io_service& io, const server::options_type& conf, wfc::io::handler handler)
  : _impl( std::make_unique<server_impl>(io, conf, handler) )
{
  
}

void server::start()
{
  _impl->start();
}

void server::stop()
{
  _impl->stop();
}
  
void server::shutdown()
{
  
}

}}}}}
