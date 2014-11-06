
#include "gateway.hpp"
#include "tcp_client.hpp"
#include "gateway_config.hpp"

#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/client.hpp>

namespace wfc{ namespace gateway{

void gateway::create(wfc::io_service& io, const gateway_config& conf)
{
  _conf = conf;
  if ( conf.enabled  )
  {
    for ( const auto& t: conf.tcp)
    {
      _tcp_clients.push_back(
          std::make_shared<tcp_client>( io, t, _jsonrpc )
      );
    }
  }
}
  
gateway::gateway(wfc::io_service& io, const gateway_config& conf, jsonrpc_ptr jsonrpc)
  : _io_service(io)
  , _conf( conf )
  , _jsonrpc(jsonrpc)
{
  this->create( io, conf);
}
  
gateway::gateway(std::weak_ptr< wfc::global > global, const gateway_config& conf)
  : _global(global)
  , _io_service( global.lock()->io_service )
  , _conf( conf )
{
}
  
void gateway::reconfigure(const gateway_config& conf)
{
  _conf = conf;
}
  
void gateway::initialize( jsonrpc_ptr jsonrpc )
{
  // TODO: мягкий перезапуск
  _jsonrpc = jsonrpc;
  _tcp_clients.clear();
  this->create( _io_service, _conf);
}
  
void gateway::start()
{
  for (auto& i: _tcp_clients)
  {
    i->start();
  }
}
  
void gateway::stop()
{
  for (auto& i: _tcp_clients)
  {
    i->stop();
    i.reset();
  }
  _tcp_clients.clear();
}

}}
