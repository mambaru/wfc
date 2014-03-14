

#include <wfc/io/ip/tcp/rn/connection.hpp>
#include <wfc/io/ip/tcp/rn/connection_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

connection::~connection()
{
  //_impl.reset();
  std::cout << "connection::~connection() " << this->get_id() << std::endl;
}

  
connection::connection(connection::descriptor_type&& desc, const connection::options_type& conf, wfc::io::handler handler )
  : _impl( std::make_unique<connection_impl>(std::move(desc), conf, handler) )
{
  
}

wfc::io::io_id_t connection::get_id() const
{
  return _impl->get_id();
}

void connection::start()
{
  _impl->start();
}

void connection::stop()
{
  _impl->stop();
}

const wfc::io_service::strand& connection::strand() const
{
  return _impl->strand();
}

wfc::io_service::strand& connection::strand()
{
  return _impl->strand();
}

void connection::shutdown()
{
  
}

  

  
}}}}}
