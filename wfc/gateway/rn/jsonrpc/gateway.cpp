
#include <wfc/gateway/rn/jsonrpc/gateway.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/client.hpp>
namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{

void gateway::create(wfc::io_service& io, const gateway_config& conf, std::shared_ptr<ifactory> fact)
{
  _conf = conf;
  
  if ( !conf.tcp.empty() )
  {
    _jsonrpc_for_tcp = fact->create_for_tcp(io, conf.jsonrpc);
    for (auto& t : conf.tcp )
    {
      _tcp_clients.push_back(
          std::make_shared<client_tcp_type>( io, t, _jsonrpc_for_tcp )
      );
    }
  }
  
}
  
gateway::gateway(wfc::io_service& io, const gateway_config& conf, std::shared_ptr<ifactory> fact)
  : _io_service(io)
  , _conf( conf )
{
  this->create( io, conf, fact);
}
  
gateway::gateway(std::weak_ptr< wfc::global > global, const gateway_config& conf/*, std::shared_ptr<ifactory> fact*/)
  : _io_service( global.lock()->io_service )
  , _conf( conf )
{
}
  
void gateway::reconfigure(const gateway_config& conf)
{
  _conf = conf;
}
  
void gateway::initialize( std::shared_ptr<ifactory> fact )
{
  _tcp_clients.clear();
  this->create( _io_service, _conf, fact );
}
  
void gateway::start()
{
  if (_jsonrpc_for_tcp != nullptr)
  {
    _jsonrpc_for_tcp->start();
  }
    
  for (auto& i: _tcp_clients)
  {
    i->start();
  }
}
  
void gateway::stop()
{
  
}


}}}}
