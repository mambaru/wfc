
#include <wfc/service/rn/jsonrpc/service.hpp>
#include <wfc/jsonrpc/service.hpp>
#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
#include <wfc/io/ip/udp/rn/jsonrpc/server.hpp>

namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

void service::create(wfc::io_service& io, const service_config& conf, std::shared_ptr<ifactory> fact)
{
  _conf = conf;

  jsonrpc_options jopt;
  if ( conf.jsonrpc == nullptr )
  {
    jopt = jsonrpc_options::create();
  }
  else
  {
    jopt = *(conf.jsonrpc);
  }
  
  _jsonrpc = fact->create_for_tcp(io, jopt);

  if ( !conf.tcp.empty() )
  {
    for (auto& t : conf.tcp )
    {
      _tcp_servers.push_back(
          std::make_shared<server_tcp_type>( io, t, _jsonrpc )
      );
    }
  }
  
  if ( !conf.udp.empty() )
  {
    for (auto& t : conf.udp )
    {
      _udp_servers.push_back(
          std::make_shared<server_udp_type>( io, t, _jsonrpc )
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
  : _io_service( global.lock()->io_service)
  , _conf( conf )
{
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
  if (_jsonrpc != nullptr)
  {
    _jsonrpc->start();
  }
    
  for (auto& i: _tcp_servers)
  {
    i->start();
  }

  for (auto& i: _udp_servers)
  {
    i->start();
  }
}

/*
jsonrpc_options service::create_jsonrpc_options()
{
  ::wfc::jsonrpc::options opt;
  ::wfc::jsonrpc::options::worker wrk;
  ::wfc::jsonrpc::options::worker::strand stn;
  stn.count = 1;
  stn.methods.push_back("*");
  wrk.strands.push_back(stn);
  wrk.threads = 1;
  opt.workers.push_back(wrk);
  return opt;
}
*/
service_config service::create_config(std::string type)
{
  service_config conf;
  if ( type.find("jsonrpc") != std::string::npos )
  {
    conf.jsonrpc = std::make_shared<jsonrpc_options>( ::wfc::jsonrpc::options::create() );
  }
  
  tcp_options tcp;
  tcp.host = "0.0.0.0";
  tcp.port = "12345";
  conf.tcp.push_back(tcp);
  return conf;
}
  
void service::stop()
{
  for (auto& i: _tcp_servers)
  {
    i->stop();
    i.reset();
  }
  _tcp_servers.clear();

  for (auto& i: _udp_servers)
  {
    i->stop();
    i.reset();
  }
  _udp_servers.clear();

  
  if (_jsonrpc != nullptr)
  {
    _jsonrpc->stop();
    _jsonrpc.reset();
  }
  
}


}}}}
