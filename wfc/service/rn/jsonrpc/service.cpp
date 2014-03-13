
#include <wfc/service/rn/jsonrpc/service.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

void service::create(wfc::io_service& io, const configuration& conf, std::shared_ptr<ifactory> fact)
{
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
  
service::service(wfc::io_service& io, const configuration& conf, std::shared_ptr<ifactory> fact)
{
  this->create( io, conf, fact);
}
  
service::service(std::weak_ptr< wfc::global > global, const configuration& conf, std::shared_ptr<ifactory> fact)
{
  if (auto g = global.lock())
  {
    if (auto io = g->io_service.lock() )
    {
      this->create( *io, conf, fact);
    }
  }
  
}
  
void service::reconfigure(const configuration& )
{
  
}
  
void service::initialize( std::shared_ptr<ifactory>  )
{
  
}
  
void service::start()
{
  if (_jsonrpc_for_tcp != nullptr)
  {
    std::cout << "!!!!!!!!!!!!!!!!!!!! _jsonrpc_for_tcp->start() !!!!!!!!!!!!!!!!!!!!!" << std::endl;
    _jsonrpc_for_tcp->start();
  }
    
  for (auto& i: _tcp_servers)
  {
    std::cout << "!!!!!!!!!!!!!!!!!!!! TCP START !!!!!!!!!!!!!!!!!!!!!" << std::endl;
    i->start();
  }
}
  
void service::stop()
{
  
}


}}}}
