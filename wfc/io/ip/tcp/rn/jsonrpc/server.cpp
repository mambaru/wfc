

#include <wfc/io/ip/tcp/rn/jsonrpc/server.hpp>
#include <wfc/jsonrpc/service.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{

server::~server()
{
  //_impl.reset();
  std::cout << "server::~server()..." << std::endl;
  _server.reset();
  std::cout << "...server::~server()..." << std::endl;
}

  
server::server(wfc::io_service& io, const server::options_type& conf, std::shared_ptr<wfc::jsonrpc::service> service)
{
  using namespace std::placeholders;

  server::options_type opt = conf;
  _service = service;
  
  opt.connection.startup_handler = std::bind( &wfc::jsonrpc::service::startup_handler, _service, _1, _2, _3 );

  _server = std::make_shared<wfc::io::ip::tcp::rn::server>(
    io, opt, 
    std::bind( &wfc::jsonrpc::service::operator(), _service, _1, _2, _3 )
  );
}

void server::start()
{
  _server->start();
}

void server::stop()
{
  std::cout << "rn::jsonrpc::server::stop()..." << std::endl;
  _server->stop();
  std::cout << "rn::jsonrpc::server::stop() reset {" << std::endl;
  _server.reset();
  std::cout << "} rn::jsonrpc::server::stop() reset" << std::endl;
}
  
void server::shutdown()
{
  
}

}}}}}}
