#include "module.hpp"

namespace wfc{

/*
module::module()
  : _impl( nullptr )
{
}
*/

module::module( std::shared_ptr< ::wfc::imodule > impl)
  : _impl(impl)
{
}

::wfc::imodule::priority module::startup_priority() const
{
  return _impl->startup_priority();
}
  
::wfc::imodule::priority module::shutdown_priority() const
{
  return _impl->startup_priority();
}
  
std::string module::version() const
{
  return _impl->version();
}
  
std::string module::description() const
{
  return _impl->description();
}
  
std::string module::generate(const std::string& type) const
{
  return _impl->generate(type);
}

bool module::parse_config(const std::string& confstr)
{
  return _impl->parse_config(confstr);
}
  
void module::create( const std::string& name, std::shared_ptr< ::wfc::global> g )
{
  _impl->create(name, g);
}
  
void module::configure(const std::string& conf, const std::string& args)
{
  _impl->configure(conf, args);
}
  
void module::initialize(const std::string& args)
{
  _impl->initialize(args);
}
  
void module::start(const std::string& args)
{
  _impl->start(args);
}

void module::stop(const std::string& args)
{
  _impl->stop(args);
}
  
void module::shutdown(const std::string& args)
{
  _impl->shutdown(args);
}
  
}
