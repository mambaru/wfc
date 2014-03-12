
#include <wfc/service/rn/jsonrpc/service.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

service::service(std::weak_ptr< wfc::global > global, const configuration& conf, std::shared_ptr<ifactory> fact)
{
  if (auto g = global.lock())
  {
    if (auto io = g->io_service.lock() )
    {
      if ( !conf.tcp.empty() )
      {
        _jsonrpc_for_tcp = fact->create_for_tcp();
        for (auto& t : conf.tcp )
        {
          _tcp_servers.push_back(
              std::make_shared<server_tcp_type>( *io, t, _jsonrpc_for_tcp )
          );
        }
      }
    }
  }
  
}
  
void service::reconfigure(const configuration& )
{
  
}
  
void service::initialize( )
{
  
}
  
void service::start()
{
  if (_jsonrpc_for_tcp != nullptr)
    _jsonrpc_for_tcp->start();
    
  for (auto& i: _tcp_servers)
  {
    i->start();
  }
}
  
void service::stop()
{
  
}


}}}}
