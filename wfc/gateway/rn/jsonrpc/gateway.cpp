
#include <wfc/gateway/rn/jsonrpc/gateway.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/client.hpp>
namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{

void gateway::create(wfc::io_service& io, const gateway_config& conf, std::shared_ptr<ifactory> fact)
{
  _conf = conf;
  
  jsonrpc_options jopt;
  
  if ( conf.jsonrpc!=nullptr)
  {
    jopt = *(conf.jsonrpc);
  }
  else
  {
    jopt = jsonrpc_options::create();
  }
  
  if ( !conf.tcp.empty() )
  {
    _jsonrpc_for_tcp = fact->create_for_tcp(io, jopt);
    for (auto& t : conf.tcp )
    {
      _tcp_clients.push_back(
          std::make_shared<client_tcp_type>( io, t, _jsonrpc_for_tcp )
      );
    }
  }

  if ( !conf.pubsub.empty() )
  {
    _jsonrpc_for_pubsub = fact->create_for_pubsub(io, jopt);
    for (auto& t : conf.pubsub )
    {
      auto pg = std::make_shared<pubsub_gateway>( _global, t );
      _pubsubs.push_back(pg);
      pg->initialize( _jsonrpc_for_pubsub );
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
  : _global(global)
  , _io_service( global.lock()->io_service )
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
  _pubsubs.clear();
  this->create( _io_service, _conf, fact );
}
  
void gateway::start()
{
  if (_jsonrpc_for_tcp != nullptr)
  {
    _jsonrpc_for_tcp->start();
  }
  
  if (_jsonrpc_for_pubsub != nullptr)
  {
    _jsonrpc_for_pubsub->start();
  }
    
  for (auto& i: _tcp_clients)
  {
    i->start();
  }

  for (auto& i: _pubsubs)
  {
    i->start();
  }
}
  
void gateway::stop()
{
  
  for (auto& i: _tcp_clients)
  {
    DEBUG_LOG_BEGIN("gateway::stop() tcp client")
    i->stop();
    i.reset();
    DEBUG_LOG_END("gateway::stop() tcp client")
  }
  _tcp_clients.clear();

  for (auto& i: _pubsubs)
  {
    DEBUG_LOG_BEGIN("gateway::stop() pubsub client")
    i->stop();
    i.reset();
    DEBUG_LOG_END("gateway::stop() pubsub client")
  }
  _pubsubs.clear();
  
  if (_jsonrpc_for_tcp != nullptr)
  {
    DEBUG_LOG_BEGIN("gateway::stop() jsonrpc for tcp")
    _jsonrpc_for_tcp->stop();
    _jsonrpc_for_tcp.reset();
    DEBUG_LOG_END("gateway::stop() jsonrpc for tcp")
  }


  if (_jsonrpc_for_pubsub != nullptr)
  {
    DEBUG_LOG_BEGIN("gateway::stop() jsonrpc for pubsub")
    _jsonrpc_for_pubsub->stop();
    _jsonrpc_for_pubsub.reset();
    DEBUG_LOG_END("gateway::stop() jsonrpc for pubsub")
  }

}

gateway_config gateway::create_config(std::string type)
{
  
  gateway_config conf;
  if ( type.find("jsonrpc") != std::string::npos )
  {
    conf.jsonrpc = std::make_shared<jsonrpc_options>( ::wfc::jsonrpc::options::create() );
  }
  
  tcp_options tcp;
  tcp.host = "0.0.0.0";
  tcp.port = "12345";
  conf.tcp.push_back(tcp);
  
  pubsub_options pb;

  pb.pubsub_name="pubsub-gateway";
  pb.incoming_target="hub1";
  pb.incoming_channel="demo1";
  
  pb.outgoing_target="hub1";
  pb.outgoing_channel="demo1";
  conf.pubsub.push_back(pb);
  return conf;

  
}


}}}}
