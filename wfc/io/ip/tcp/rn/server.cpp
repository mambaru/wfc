

#include <wfc/io/ip/tcp/rn/server.hpp>
#include <wfc/io/ip/tcp/rn/server_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

server::~server()
{
  //this->stop();
}

  
server::server(wfc::io_service& io, const server::options_type& conf/*, wfc::io::incoming_handler handler*/)
  : _impl( std::make_unique<server_impl>(io, conf/*, handler*/) )
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
