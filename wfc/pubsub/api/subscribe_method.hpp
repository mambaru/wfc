#pragma once

#include <wfc/pubsub/api/subscribe_json.hpp>
#include <wfc/pubsub/api/subscribe.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <wfc/pubsub/api/tags.hpp>
#include <wfc/pubsub/ipubsub.hpp>

#include <wfc/jsonrpc/errors/error.hpp>
#include <memory>

namespace wfc{ namespace pubsub{

// {"method":"publish", "params":{"channel":"test"}, "id":1}
// {"method":"subscribe", "params":"test", "id":1}
struct subscribe_method
{
  const char* name() const { return "subscribe";}
  
  typedef request::subscribe_json::type  invoke_request;
  typedef response::subscribe_json::type invoke_response;
  typedef request::subscribe_json::type  invoke_notify;
  
  typedef std::function< 
    callback_status(
      std::unique_ptr<response::subscribe>, 
      std::unique_ptr<wfc::jsonrpc::error>
    )
  > request_callback;
  
  typedef std::function< 
    callback_status(
      std::unique_ptr< response::publish> req, 
      std::unique_ptr< wfc::jsonrpc::error > err
    )
  > publish_callback;
  
  template<typename T>
  void request(T& t, ipubsub::request_subscribe_ptr req, int /*id*/, request_callback callback )
  {
    if ( auto target = t.context().target.lock() )
    {
      target->subscribe( 
        std::move(req),
        this->subscribe_handler(t),
        this->callback_handler(callback)
      ); 
    }
    else
    {
      std::cout << "subscribe NULL" << std::endl;
      if (callback != nullptr )
        callback( nullptr, jsonrpc::server_configuration_error() );
    }
  }

  template<typename T>
  void notify(T& t, ipubsub::request_subscribe_ptr req)
  {
    this->request(t,  std::move(req), 0, nullptr);
    /*if ( auto target = t.context().target.lock() )
    {
      target->subscribe( 
        std::move(req),
        this->subscribe_handler(t),
        nullptr
      ); 
    }*/
  }
  
private:
  
  ipubsub::subscribe_callback callback_handler(request_callback callback)
  {
    ipubsub::subscribe_callback result = nullptr;
    
    if (callback !=nullptr)
    {
      result =
        [callback](std::unique_ptr< response::subscribe > req) 
        {
          return callback( std::move(req), nullptr);
        };
    }
    return result;
  }
  
  template<typename T>
  ipubsub::subscriber_function subscribe_handler(T& t) 
  {
    return t.owner().template callback<wfc::callback_status, ipubsub::request_publish_ptr, ipubsub::publish_callback>
    (
      [&t, this](ipubsub::request_publish_ptr req, ipubsub::publish_callback pc ) -> wfc::callback_status
      {
        if ( pc!=nullptr )
        {
          // Отправляем запросом, и ждем подверждения
          t.get_aspect().template get<_publish_>().request
          (
            t, 
            std::move(req),
            this->publish_handler(pc)
          ); // t.get_aspect().template get<_publish_>()
        }
        else
        {
          t.get_aspect().template get<_publish_>().notify(t, std::move(req));
        }
        return callback_status::ready;
      }
    ); // t.owner().callback
  }
  
  publish_callback publish_handler(ipubsub::publish_callback pc)
  {
    return
      [pc]( ipubsub::response_publish_ptr req, std::unique_ptr< wfc::jsonrpc::error > err )
      {
        if ( err != nullptr )
          req->status = pubsub::status::bad_gateway;
        return pc( std::move(req) ); 
      };
  }
  
};

}}