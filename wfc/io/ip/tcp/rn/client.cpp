
#include <wfc/io/ip/tcp/rn/client.hpp>
#include <wfc/io/ip/tcp/rn/client_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

client::~client()
{
}

  
client::client(wfc::io_service& io, const options_type& conf/*, wfc::io::handler handler*/ )
  : _impl( std::make_unique<client_impl>( io, conf/*, handler*/) )
{
  
}

wfc::io::io_id_t client::get_id() const
{
  return _impl->get_id();
}

void client::start()
{
  _impl->start();
}

void client::stop(std::function<void()> finalize)
{
  _impl->stop(finalize);
}
  
void client::shutdown()
{
  
}

  

  
}}}}}
