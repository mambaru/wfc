
#include <wfc/service/rn/jsonrpc/service_list.hpp>
namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

void service_list::create(wfc::io_service& io, const service_list_config& conf, std::shared_ptr<ifactory> fact)
{
  _conf = conf;
  
  for (const auto& c: _conf.services)
    _service_list.push_back( std::make_shared<service>( io, c, fact) );
}
  
service_list::service_list(wfc::io_service& io, const service_list_config& conf, std::shared_ptr<ifactory> fact)
  : _io_service(io)
  , _conf( conf )
{
  this->create( io, conf, fact);
}
  
service_list::service_list(std::weak_ptr< wfc::global > global, const service_list_config& conf)
  : _io_service( global.lock()->io_service )
  , _conf( conf )
{
}
  
void service_list::reconfigure(const service_list_config& conf)
{
  _conf = conf;
}
  
void service_list::initialize( std::shared_ptr<ifactory> fact )
{
  _service_list.clear();
  this->create( _io_service, _conf, fact );
}
  
void service_list::start()
{
  for (auto& i: _service_list)
  {
    i->start();
  }
}
  
void service_list::stop()
{
  for (auto& i: _service_list)
  {
    i->stop();
  }
}

}}}}
