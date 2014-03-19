
#include <wfc/gateway/rn/jsonrpc/gateway_list.hpp>

namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{

void gateway_list::create(wfc::io_service& io, const gateway_list_config& conf, std::shared_ptr<ifactory> fact)
{
  _conf = conf;
  
  for (const auto& c: _conf.gateways)
    _gateway_list.push_back( std::make_shared<gateway>( io, c, fact) );
}
  
gateway_list::gateway_list(wfc::io_service& io, const gateway_list_config& conf, std::shared_ptr<ifactory> fact)
  : _io_service(io)
  , _conf( conf )
{
  this->create( io, conf, fact);
}
  
gateway_list::gateway_list(std::weak_ptr< wfc::global > global, const gateway_list_config& conf)
  : _io_service( global.lock()->io_service )
  , _conf( conf )
{
}
  
void gateway_list::reconfigure(const gateway_list_config& conf)
{
  _conf = conf;
}
  
void gateway_list::initialize( std::shared_ptr<ifactory> fact )
{
  _gateway_list.clear();
  this->create( _io_service, _conf, fact );
}
  
void gateway_list::start()
{
  for (auto& i: _gateway_list)
  {
    i->start();
  }
}
  
void gateway_list::stop()
{
  for (auto& i: _gateway_list)
  {
    i->stop();
  }
}

}}}}
