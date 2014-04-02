#include <wfc/pubsub/pubsub_gateway.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify_json.hpp>

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
      if ( !_options.pubsub_name.empty() )
        ps->set(_options.pubsub_name, this->shared_from_this() );
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
  /// std::bind(&pubsub_gateway::process_outgoing, this, std::placeholders::_1),
  if ( auto t = _incoming_target.lock() )
  {
    std::cout << "void pubsub_gateway::start()" << std::cout;
    auto names = _jsonrpc->get_methods();
    for (auto &n : names)
    {
      std::cout << n << std::endl;
      auto req = std::make_unique<request::subscribe>();
      req->channel = _options.outgoing_channel + "." + n;
      //auto callback = std::bind( &pubsub_gateway::publish, this, std::placeholders::_1, std::placeholders::_2 );
      t->subscribe(std::move(req), nullptr, super::get_id(), [this](request_publish_ptr req, publish_callback cb)
      {
        this->publish(std::move(req), cb);
      } );
    }
  }
}

void pubsub_gateway::stop()
{
  if ( auto t = _incoming_target.lock() )
  {
    t->describe(super::get_id());
  }
}


void pubsub_gateway::process_outgoing( ::wfc::io::data_ptr d)
{
  auto pd = std::make_shared< ::wfc::io::data_ptr>( std::move(d) );
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

void pubsub_gateway::describe( size_t )
{
  
}
  
void pubsub_gateway::subscribe(request_subscribe_ptr, subscribe_callback, size_t , publish_handler )
{
  
}

void pubsub_gateway::publish(request_publish_ptr req, publish_callback cb)
{
  std::cout << "PUBLISH!!! " << std::string(req->content->begin(), req->content->end()) << " callback=" << (cb!=nullptr) << std::endl;
  
  bool allow = (_options.incoming_channel.size() <= req->channel.size());
  allow = allow && (0 == req->channel.compare(0, _options.incoming_channel.size(), _options.incoming_channel ));
  std::cout << "-1-" << allow<< std::endl;
  if ( !allow )
  {
    std::cout << "-1.1-" << std::endl;
    if ( cb!=nullptr )
    {
      auto resp = std::make_unique<response::publish>();
      resp->status = status::forbidden;
      cb( std::move(resp) );
      std::cout << "-1.2-" << std::endl;
    }
    std::cout << "-1.3-" << std::endl;
    // TODO: LOG
    return;
  }
  
  std::cout << "-2-" << std::endl;
  if ( cb == nullptr )
  {
    ::wfc::jsonrpc::outgoing_notify< ::wfc::io::data_type> notify;
    if ( req->content!=nullptr )
    {
      notify.params = std::move( req->content );
    }
    std::cout << "-3-" << std::endl;
    typedef ::wfc::jsonrpc::outgoing_notify_json< 
      ::wfc::json::raw_value< ::wfc::io::data_type> 
    >::serializer serializer;
    auto data = std::make_unique< ::wfc::io::data_type >();
    if ( notify.params!=nullptr )
    {
      data->reserve(notify.params->size() + 200 );
    }
    notify.method.assign(req->channel.begin() + _options.incoming_channel.size() + 1, req->channel.end());
    std::cout << "-4-" << std::endl;
    serializer()( notify, std::inserter(*data, data->end() ) );
    std::cout << "convert to: " << std::string(data->begin(), data->end()) << std::endl;
    (*_jsonrpc)( std::move(data), super::get_id(), nullptr );
  }
  //std::cout << "PUBLISH!!! " << std::string(req->content->begin(), req->content->end()) << " callback=" << (cb!=nullptr) << std::endl;
}

void pubsub_gateway::publish(request_multi_publish_ptr, multi_publish_callback)
{
  
}
  
void pubsub_gateway::load(request_load_ptr, load_callback )
{
  
}

void pubsub_gateway::load(request_multi_load_ptr, multi_load_callback )
{
  
}

void pubsub_gateway::query(request_query_ptr, query_callback )
{
  
}
  
void pubsub_gateway::notify(request_notify_ptr, notify_callback )
{
  
}

  
}}
