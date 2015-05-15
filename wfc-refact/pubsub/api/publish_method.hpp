#pragma once

#include <wfc/pubsub/api/publish_json.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <wfc/jsonrpc/errors/error.hpp>

#include <memory>

namespace wfc{ namespace pubsub{

struct publish_method
{
  //
  // Сделать обертку TRY..CATCH и сделать универсальным
  // Имя для logger'a передавать в контексте jsonrpc или пользовательском ???
  //
  const char* name() const { return "publish";}
  
  typedef request::publish_json::type  invoke_request;
  typedef response::publish_json::type invoke_response;
  typedef request::publish_json::type  invoke_notify;

  
  typedef request::publish_json::type  call_request;
  typedef response::publish_json::type call_response;
  typedef request::publish_json::type  call_notify;
  
  template<typename T, typename F>
  void request(T& t, std::unique_ptr< request::publish > req, int /*id*/, F callback)
  {
    if ( auto pb = t.context().target.lock() )
    {
      pb->publish( 
        std::move(req), 
        [callback](std::unique_ptr< response::publish > req) 
        {
          return callback( std::move(req), nullptr);
        }
      );
    }
    else
    {
      // callback == nullptr - это будет  
      if ( callback != nullptr )
      {
        callback( nullptr, jsonrpc::server_configuration_error() );
      }
    }
  }

  template<typename T>
  void notify(T& t, std::unique_ptr< request::publish > req)
  {
    if ( auto pb = t.context().target.lock() )
    {
      pb->publish( std::move(req), nullptr );
    }
    /*
    std::reverse(req->begin(), req->end());
    callback( std::move(req), nullptr );
    t.get_aspect().template get<_reverse_>().notify(t, std::make_unique< std::vector< int > >());
    */
  }
};

}}