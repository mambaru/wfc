#include <wfc/pubsub/pubsub_gateway.hpp>

namespace wfc{ namespace pubsub{
  
pubsub_gateway::~pubsub_gateway()
{
}

pubsub_gateway::pubsub_gateway( std::weak_ptr< wfc::global > global, const options_type& conf/*, std::shared_ptr<wfc::jsonrpc::service> service*/)
  : super(global.lock()->io_service, conf)
  , _global(global)
  , _options(conf)
  //, _jsonrpc( service)
  //, _io_id( ::wfc::io::create_id() )
{
}

void pubsub_gateway::initialize( std::shared_ptr< ::wfc::jsonrpc::service> jsonrpc)
{
  _jsonrpc = jsonrpc;
  //_io_id = ::wfc::io::create_id();
  _jsonrpc->startup_handler(
    super::get_id(),
    [](wfc::io::data_ptr d)
    {
      
    },
    []( std::function< void(wfc::io::io_id_t)>)
    {
      
    }
  );
  
}

void pubsub_gateway::start()
{
}

void pubsub_gateway::process( ::wfc::io::data_ptr d )
{
  
}
  
}}
