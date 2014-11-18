module::module()
  : _impl( std::make_shared<module_impl>() )
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
  
void module::configure(const std::string& conf)
{
  _impl->configure(conf);
}
  
void module::initialize()
{
  _impl->initialize();
}
  
void module::start()
{
  _impl->start();
}

void module::stop()
{
  _impl->stop();
}
  
void module::shutdown()
{
  _impl->shutdown();
}
  
