
#include "tcp_client.hpp"
#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/client.hpp>

namespace wfc{ namespace gateway{

void tcp_client::create(wfc::io_service& io, const tcp_options& conf)
{
  _conf = conf;
  if ( conf.enabled && conf.count > 0 )
  {
    for (size_t i=0; i < conf.count; ++i )
    {
      _tcp_clients.push_back(
          std::make_shared<client_type>( io, conf, _jsonrpc )
      );
    }
  }
}
  
tcp_client::tcp_client(wfc::io_service& io, const tcp_options& conf, rpc_ptr jsonrpc)
  : _io_service(io)
  , _conf( conf )
  , _jsonrpc(jsonrpc)
{
  this->create( io, conf);
}
  
tcp_client::tcp_client(std::shared_ptr< wfc::global > global, const tcp_options& conf)
  : _global(global)
  , _io_service( global->io_service )
  , _conf( conf )
{
}
  
void tcp_client::reconfigure(const tcp_options& conf)
{
  _conf = conf;
}
  
void tcp_client::initialize( rpc_ptr jsonrpc )
{
  // TODO: мягкий перезапуск
  _jsonrpc = jsonrpc;
  _tcp_clients.clear();
  this->create( _io_service, _conf);
}
  
void tcp_client::start()
{
  for (auto& i: _tcp_clients)
  {
    i->start();
  }
}
  
void tcp_client::stop()
{
  for (auto& i: _tcp_clients)
  {
    i->stop(nullptr);
    i.reset();
  }
  _tcp_clients.clear();
}

}}
