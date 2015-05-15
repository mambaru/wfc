

#include <wfc/io/ip/tcp/rn/jsonrpc/client.hpp>
#include <wfc/jsonrpc/service.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace jsonrpc{

client::~client()
{
  //_impl.reset();
}

  
client::client(io_service_type& io, const client::options_type& conf, std::shared_ptr<wfc::jsonrpc::service> service)
{
  using namespace std::placeholders;

  client::options_type opt = conf;
  _service = service;
  
  opt.connection.startup_handler = std::bind( &wfc::jsonrpc::service::create_handler, _service, _1, _2, _3 );
  opt.connection.incoming_handler = std::bind( &wfc::jsonrpc::service::operator(), _service, _1, _2, _3 );
  _client = std::make_shared<wfc::io::ip::tcp::rn::client>(io, opt);
}

void client::start()
{
  _client->start();
}

void client::stop(std::function<void()> finalize)
{
  _client->stop(finalize);
}
  
void client::shutdown()
{
  
}

}}}}}}
