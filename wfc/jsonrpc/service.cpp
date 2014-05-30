
#include <wfc/jsonrpc/service/service_impl.hpp>
#include <wfc/jsonrpc/service.hpp>

namespace wfc{ namespace jsonrpc{

service::service( ::wfc::io_service& io_service, const options_type& opt, const handler_base& handler)
  : _impl( std::make_shared<service_impl>(io_service, opt, handler) )
{
}

void service::startup_handler( ::wfc::io::io_id_t io_id,  ::wfc::io::outgoing_handler_t writer,  ::wfc::io::add_shutdown_handler_t add_shutdown )
{
  _impl->startup_handler(io_id, writer, add_shutdown);
}

void service::operator()(  io::data_ptr d,  io::io_id_t io_id,  ::wfc::io::outgoing_handler_t outgoing_handler)
{
  _impl->operator ()( std::move(d), io_id, outgoing_handler);
}

std::vector<std::string> service::get_methods() const
{
  return _impl->get_methods();
}

void service::start()
{
  _impl->start();
}

void service::stop()
{
  _impl->stop();
}


}} 