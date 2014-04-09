
#include <wfc/jsonrpc/service/service_impl.hpp>
#include <wfc/jsonrpc/service.hpp>

namespace wfc{ namespace jsonrpc{
  

service::service( ::wfc::io_service& io_service, const options_type& opt, const handler_base& handler)
  : _impl( std::make_shared<service_impl>(io_service, opt, handler) )
{
}

void service::startup_handler( ::wfc::io::io_id_t io_id,  ::wfc::io::callback writer,  ::wfc::io::add_shutdown_handler add_shutdown )
{
  _impl->startup_handler(io_id, writer, add_shutdown);
}

void service::operator()(  ::wfc::io::data_ptr d,  ::wfc::io::io_id_t id,  ::wfc::io::callback callback)
{
  _impl->operator ()( std::move(d), id, callback);
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