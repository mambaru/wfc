

#include <wfc/io/ip/tcp/rn/client.hpp>
#include <wfc/io/ip/tcp/rn/client_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

client::~client()
{
  //_impl.reset();
  std::cout << "client::~client() " << this->get_id() << std::endl;
}

  
client::client(client::descriptor_type&& desc, const client::options_type& conf, wfc::io::handler handler )
  : _impl( std::make_unique<client_impl>(std::move(desc), conf, handler) )
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

void client::stop()
{
  _impl->stop();
}
  
void client::shutdown()
{
  
}

  

  
}}}}}
