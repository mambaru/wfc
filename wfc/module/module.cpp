
#include <wfc/module/module.hpp>

namespace wfc{

module::~module()
{
  
}
  
module::module(std::shared_ptr<imodule> p)
  : _impl(p)
{
  
}

std::string module::name() const
{
  return _impl->name();
}

std::string module::description() const
{
  return _impl->description();
}

std::vector< std::shared_ptr<iobject> > module::objects() const
{
  return _impl->objects();
}

void module::create( std::shared_ptr<wfcglobal> g)
{
  return _impl->create(g);
}

void module::start(const std::string& arg)
{
  return _impl->start(arg);
}

void module::stop(const std::string& arg)
{
  return _impl->stop(arg);
}

}
