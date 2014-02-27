#include "server.hpp"
#include "server/server.hpp"
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{
  
server::server( wfc::io_service& io)
  : _server( std::make_unique<impl::server>(io) )
{
  
}

server::server( std::weak_ptr< wfc::global > g, const server::config& conf )
{
  _server = std::make_unique<impl::server>( *(g.lock()->io_service.lock()), conf );
}
  
void server::reconfigure(const server::config& conf)
{
  _server->reconfigure(conf);
}

void server::configure(const server::config& conf)
{
  _server->configure(conf);
}

void server::initialize( server::handler /*h*/ )
{
  // todo: handler
}

void server::start()
{
  _server->start();
}

void server::stop()
{
  
}

}}}}}
