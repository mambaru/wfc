

#include <wfc/io/ip/udp/rn/connection.hpp>
#include <wfc/io/ip/udp/rn/connection_impl.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{

connection::~connection()
{
}

  
connection::connection(connection::descriptor_type&& desc, const connection::options_type& conf )
  : _impl( std::make_shared<connection_impl>(std::move(desc), conf) )
{
  
}

io_id_t connection::get_id() const
{
  return _impl->get_id();
}

void connection::start()
{
  _impl->start();
}

void connection::stop(std::function<void()> finalize)
{
  _impl->stop(finalize);
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
