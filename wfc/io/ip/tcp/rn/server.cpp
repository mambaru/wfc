

#include <wfc/io/ip/tcp/rn/server.hpp>
#include <wfc/io/ip/tcp/rn/server_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

server::~server()
{
  
  std::cout << "tcp::rn::server::~server()..." << std::endl;
  if ( _impl!=nullptr )
    _impl->stop(nullptr);
  std::cout << "tcp::rn::server::~server() reset.." << std::endl;
  _impl.reset();
  std::cout << "...tcp::rn::server::~server()" << std::endl;
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
  std::cout << "server::stop()" << std::endl;
  _impl->stop(nullptr);
  std::cout << "server::stop() _impl.reset() {" << std::endl;
  _impl.reset();
  std::cout << "} server::stop() _impl.reset()" << std::endl;
}
  
void server::shutdown()
{
  
}

}}}}}
