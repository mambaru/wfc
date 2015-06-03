
#include <wfc/module/object.hpp>

namespace wfc{

object::~object()
{
  
}
  
object::object(std::shared_ptr<iobject> p)
  : _impl(p)
{
  
}

std::string object::name() const
{
  return _impl->name();
}

std::string object::description() const
{
  return _impl->description();
}

std::string object::interface_name() const
{
  return _impl->interface_name();
}

std::string object::generate(const std::string& type) const
{
  return _impl->generate(type);
}

bool object::parse(const std::string& strjson)
{
  return _impl->parse(strjson);
}

void object::configure(const std::string& strjson, const std::string& arg)
{
  return _impl->configure(strjson, arg);
}


void object::create( std::shared_ptr<wfcglobal> g)
{
  return _impl->create(g);
}

void object::start(const std::string& arg)
{
  return _impl->start(arg);
}

void object::stop(const std::string& arg)
{
  return _impl->stop(arg);
}

void object::startup_io(io_id_t io_id, outgoing_handler_t handler)
{
  return _impl->startup_io( std::move(io_id), std::move(handler) );
}

void object::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
{
  return _impl->perform_io(std::move(d), std::move(io_id),  std::move(handler) );
}

void object::shutdown_io(io_id_t io_id)
{
  return _impl->shutdown_io(std::move(io_id));
}

}
