

#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
#include <wfc/jsonrpc/service.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{

server::~server()
{
}
  
server::server(wfc::io_service& io, const server::options_type& conf, std::shared_ptr<wfc::jsonrpc::service> service)
{
  using namespace std::placeholders;

  server::options_type opt = conf;
  _service = service;
  
  opt.connection.startup_handler = std::bind( & ::wfc::jsonrpc::service::create_handler, _service, _1, _2, _3 );
  opt.incoming_handler = std::bind( & ::wfc::jsonrpc::service::operator(), _service, _1, _2, _3 );

  _server = std::make_shared< ::wfc::io::ip::tcp::rn::server>(io, opt);
}

void server::start()
{
  _server->start();
}

void server::stop()
{
  _server->stop();
  _server.reset();
}
  
void server::shutdown()
{
  
}

}}}}}}
