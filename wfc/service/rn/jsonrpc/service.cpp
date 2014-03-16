
#include <wfc/service/rn/jsonrpc/service.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

void service::create(wfc::io_service& io, const service_config& conf, std::shared_ptr<ifactory> fact)
{
  _conf = conf;
  
  if ( !conf.tcp.empty() )
  {
    _jsonrpc_for_tcp = fact->create_for_tcp(io, conf.jsonrpc);
    for (auto& t : conf.tcp )
    {
      _tcp_servers.push_back(
          std::make_shared<server_tcp_type>( io, t, _jsonrpc_for_tcp )
      );
    }
  }
  
}
  
service::service(wfc::io_service& io, const service_config& conf, std::shared_ptr<ifactory> fact)
  : _io_service(io)
  , _conf( conf )
{
  this->create( io, conf, fact);
}
  
service::service(std::weak_ptr< wfc::global > global, const service_config& conf/*, std::shared_ptr<ifactory> fact*/)
  : _io_service( *(global.lock()->io_service.lock()) )
  , _conf( conf )
{
  /*if (auto g = global.lock())
  {
    if (auto io = g->io_service.lock() )
    {
      this->create( *io, conf, fact);
    }
  }
  */
  
}
  
void service::reconfigure(const service_config& conf)
{
  _conf = conf;
}
  
void service::initialize( std::shared_ptr<ifactory> fact )
{
  _tcp_servers.clear();
  this->create( _io_service, _conf, fact );
}
  
void service::start()
{
  if (_jsonrpc_for_tcp != nullptr)
  {
    _jsonrpc_for_tcp->start();
  }
    
  for (auto& i: _tcp_servers)
  {
    i->start();
  }
}
  
void service::stop()
{
  
}


}}}}
