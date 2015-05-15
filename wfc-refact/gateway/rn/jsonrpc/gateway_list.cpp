
#include <wfc/gateway/rn/jsonrpc/gateway_list.hpp>

namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{

  /*
void gateway_list::create(wfc::io_service& io, const gateway_list_config& conf, std::shared_ptr<ifactory> fact)
{
  for (const auto& c: _conf.gateways)
    _gateway_list.push_back( std::make_shared<gateway>( io, c, fact) );
}
*/
  
gateway_list::gateway_list(io_service_type& io, const gateway_list_config& conf, std::shared_ptr<ifactory> fact)
  : _io_service(io)
  , _conf( conf )
{
  //this->create( io, conf, fact);
  
  for (const auto& c: _conf.gateways)
    _gateway_list.push_back( std::make_shared<gateway>( io, c, fact) );

}
  
gateway_list::gateway_list(std::weak_ptr< wfc::global > global, const gateway_list_config& conf)
  : _global(global)
  , _io_service( global.lock()->io_service )
  , _conf( conf )
{
  for (const auto& c: _conf.gateways)
    _gateway_list.push_back( std::make_shared<gateway>( _global, c ) );

}
  
void gateway_list::reconfigure(const gateway_list_config& conf)
{
  _conf = conf;
  // TODO: Удалить и переконфигурировать основные параметры
}
  
void gateway_list::initialize( std::shared_ptr<ifactory> fact )
{
  for ( auto& i : _gateway_list )
    i->initialize( fact );
  /*
  _gateway_list.clear();
  this->create( _io_service, _conf, fact );
  */
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
    i.reset();
  }
  _gateway_list.clear();
}

void gateway_list::shutdown()
{
  this->stop();
}

gateway_list_config gateway_list::create_config(std::string type)
{
  gateway_list_config conf;
  conf.gateways.push_back( gateway::create_config(type) );
  return conf;
}

}}}}
