
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

}
