#include <wfc/pubsub/pubsub_gateway.hpp>
#include <wfc/jsonrpc/incoming/incoming_holder.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify.hpp>
#include <wfc/jsonrpc/outgoing/outgoing_notify_json.hpp>

namespace wfc{ namespace pubsub{
  
pubsub_gateway::~pubsub_gateway()
{
  DEBUG_LOG_MESSAGE("pubsub_gateway::~pubsub_gateway()")
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
  if ( auto t = _incoming_target.lock() )
  {
    auto names = _jsonrpc->get_methods();
    for (auto &n : names)
    {
      std::cout << n << std::endl;
      auto req = std::make_unique<request::subscribe>();
      req->channel = _options.incoming_channel + "." + n + _options.subscribe_suffix;
      //auto callback = std::bind( &pubsub_gateway::publish, this, std::placeholders::_1, std::placeholders::_2 );
      t->subscribe(std::move(req), nullptr, super::get_id(), [this](request_publish_ptr req, publish_callback cb)
      {
        if ( req==nullptr )
        {
          this->publish(nullptr, cb);
          return;
        }
        
        size_t channel_len = req->channel.size();
        size_t suffix_len = this->_options.subscribe_suffix.size();
        
        if ( suffix_len!=0 && suffix_len < channel_len )
        {
          if ( 0 == req->channel.compare( channel_len - suffix_len, suffix_len, this->_options.subscribe_suffix ) )
          {
            req->channel.resize( channel_len - suffix_len );
          }
        }
        
        if ( req!=nullptr && req->content!=nullptr)  
        {
          TRACE_LOG_MESSAGE("pubsub_gateway::publish: -subscribe- [[" << std::string( req->content->begin(), req->content->end() ) << "]" << req->channel << ", " << suffix_len)
        }
        else
        {
          TRACE_LOG_MESSAGE("pubsub_gateway::publish: -subscribe- [[null]")
        }
        this->publish(std::move(req), cb);
      } );
    }
  }
}

void pubsub_gateway::stop()
{
  DEBUG_LOG_BEGIN("pubsub_gateway::stop()")
  if ( auto t = _incoming_target.lock() )
  {
    DEBUG_LOG_MESSAGE("pubsub_gateway::stop() describe")
    t->describe(super::get_id());
  }
  
  _jsonrpc.reset();
  DEBUG_LOG_END("pubsub_gateway::stop()")
  
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
  TRACE_LOG_MESSAGE("wfc::pubsub::gateway process outgoing: [[" << std::string( d->begin(), d->end() ) << "]");
  ::wfc::jsonrpc::incoming_holder holder( std::move(d) );
  
  if ( holder.is_request() )
  {
    // TODO: custom_request он же query
    abort();
  }
  else if ( holder.is_notify() )
  {
    
    if ( auto t = _outgoing_target.lock() )
    {
      request::publish tmp;
      auto ntf = std::make_unique< request::publish >(tmp);
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
  {
    /// "...other" << std::endl;
  }
  
}

void pubsub_gateway::describe( size_t )
{
  
}
  
void pubsub_gateway::subscribe(request_subscribe_ptr, subscribe_callback, size_t , publish_handler )
{
  
}

void pubsub_gateway::describe(request_describe_ptr, describe_callback, size_t)
{
  
}

void pubsub_gateway::publish(request_publish_ptr req, publish_callback cb)
{
  if ( req!=nullptr && req->content!=nullptr)  
  {
    TRACE_LOG_MESSAGE("pubsub_gateway::publish: [[" << std::string( req->content->begin(), req->content->end() ) << "]")
  }
  
  bool allow = (_options.incoming_channel.size() <= req->channel.size());
  allow = allow && (0 == req->channel.compare(0, _options.incoming_channel.size(), _options.incoming_channel ));
  if ( !allow )
  {
    if ( cb!=nullptr )
    {
      auto resp = std::make_unique<response::publish>();
      resp->status = status::forbidden;
      cb( std::move(resp) );
    }
    // TODO: LOG
    return;
  }
  
  wfc::io::callback io_cb = nullptr;
  
  if ( cb != nullptr ) io_cb = [cb](wfc::io::data_ptr d)
  {
    ::wfc::jsonrpc::incoming_holder holder( std::move(d) );
    auto resp = std::make_unique<response::publish>();
    if ( holder.is_response() )
    {
      resp->status = status::ready;
    }
    else
    {
      resp->status = status::internal_error;
    }
    cb( std::move(resp) );
  };
  
  ::wfc::jsonrpc::outgoing_notify< ::wfc::io::data_type> notify;
  if ( req->content!=nullptr )
  {
    notify.params = std::move( req->content );
  }
  
  typedef ::wfc::jsonrpc::outgoing_notify_json< 
    ::wfc::json::raw_value< ::wfc::io::data_type> 
  >::serializer serializer;
  auto data = std::make_unique< ::wfc::io::data_type >();
  if ( notify.params!=nullptr )
  {
    data->reserve(notify.params->size() + 200 );
  }
  notify.method.assign(req->channel.begin() + _options.incoming_channel.size() + 1, req->channel.end());
  
  serializer()( notify, std::inserter(*data, data->end() ) );
  
  TRACE_LOG_MESSAGE("pubsub_gateway::publish process incoming: [[" << std::string( data->begin(), data->end() ) << "]");
  (*_jsonrpc)( std::move(data), super::get_id(), io_cb );
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
