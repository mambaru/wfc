
#include <wfc/module/component.hpp>

namespace wfc{

component::~component()
{
}

component::component(const std::shared_ptr<icomponent>& p)
  : _impl(p)
{
}

std::string component::name() const
{
  return _impl->name();
}

std::string component::description() const
{
  return _impl->description();
}

std::string component::interface_name() const
{
  return _impl->interface_name();
}

std::string component::generate(const std::string& type)
{
  return _impl->generate(type);
}

bool component::parse(const std::string& strjson, json::json_error* err)
{
  return _impl->parse(strjson, err);
}

bool component::configure(const std::string& strjson, json::json_error* err)
{
  return _impl->configure(strjson, err);
}

void component::create( std::shared_ptr<wfcglobal> g)
{
  return _impl->create(g);
}

void component::start(const std::string& arg)
{
  return _impl->start(arg);
}

void component::stop(const std::string& arg)
{
  return _impl->stop(arg);
}

void component::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  return _impl->reg_io( std::move(io_id), itf );
}

void component::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  return _impl->perform_io(std::move(d), std::move(io_id),  std::move(handler) );
}

void component::unreg_io(io_id_t io_id)
{
  return _impl->unreg_io(std::move(io_id));
}

}
