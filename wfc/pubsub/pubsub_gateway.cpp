#include <wfc/pubsub/pubsub_gateway.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>

namespace wfc{ namespace pubsub{
  
pubsub_gateway::~pubsub_gateway()
{
}

pubsub_gateway::pubsub_gateway( std::weak_ptr< wfc::global > global, const options_type& conf)
  : super(global.lock()->io_service, conf)
  , _global(global)
  , _options(conf)
{
}

void pubsub_gateway::initialize( std::shared_ptr< ::wfc::jsonrpc::service> jsonrpc)
{
  _jsonrpc = jsonrpc;
  
  if ( auto g = _global.lock() )
  {
    if ( auto ps = g->pubsubs.lock() )
    {
      _incoming_target = ps->get( _options.incoming_target );
      _outgoing_target = ps->get( _options.outgoing_target );
    }
  }
  
  
  _jsonrpc->startup_handler
  (
    super::get_id(),
    std::bind(&pubsub_gateway::process_outgoing, this, std::placeholders::_1)
    /*[](wfc::io::data_ptr d)
    {
      
    }*/,
    []( std::function< void(wfc::io::io_id_t)>)
    {
      
    }
  );
  
}

void pubsub_gateway::start()
{
}

void pubsub_gateway::process_outgoing( ::wfc::io::data_ptr d)
{
  auto pd = std::make_shared<::wfc::io::data_ptr>( std::move(d) );
  super::post([this,pd](){
    this->process_outgoing_( std::move(*pd) );
  });
}

void pubsub_gateway::process_outgoing_( ::wfc::io::data_ptr d )
{
  ::wfc::jsonrpc::incoming_holder holder( std::move(d) );
  
  if ( holder.is_request() )
  {
    // TODO: custom_request он же query
  }
  else if ( holder.is_notify() )
  {
    // TODO: publish to outgoing_target
  }
  
}
  
}}
