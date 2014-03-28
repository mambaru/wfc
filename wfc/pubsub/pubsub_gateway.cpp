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
  super::create(*this);
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
    std::bind(&pubsub_gateway::process_outgoing, this, std::placeholders::_1),
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
  std::cout <<  "void pubsub_gateway::process_outgoing_( ::wfc::io::data_ptr d ) " << d->size() <<  std::endl;
  std::cout << std::string(d->begin(), d->end()) << std::endl;
  ::wfc::jsonrpc::incoming_holder holder( std::move(d) );
  
  if ( holder.is_request() )
  {
    std::cout << "...query" << std::endl;
    // TODO: custom_request он же query
  }
  else if ( holder.is_notify() )
  {
    std::cout << "...notify" << std::endl;
    if ( auto t = _outgoing_target.lock() )
    {
      std::cout << "...notify send ..." << std::endl;
      request::publish tmp;
      std::cout << "...notify send 2 ..." << std::endl;
      auto ntf = std::make_unique< request::publish >(tmp);
      std::cout << "...notify send 3 ..." << std::endl;
      
      //auto ntf = std::unique_ptr<request::publish>( new request::publish() );
      auto method = holder.method();
      if ( method.empty() )
      {
        ntf->channel = _options.outgoing_channel;
      }
      else
      {
        ntf->channel = _options.outgoing_channel + "." + std::move(method);
      }
      
      ntf->content = std::move(holder.acquire_params());
      t->publish( std::move(ntf), nullptr );
    }
  }
  else
    std::cout << "...other" << std::endl;
  
}
  
}}
