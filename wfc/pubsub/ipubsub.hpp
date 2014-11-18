#pragma once

#include <wfc/pubsub/api/publish.hpp>
#include <wfc/pubsub/api/multi_publish.hpp>
#include <wfc/pubsub/api/load.hpp>
#include <wfc/pubsub/api/multi_load.hpp>
#include <wfc/pubsub/api/subscribe.hpp>
#include <wfc/pubsub/api/describe.hpp>
#include <wfc/pubsub/api/query.hpp>
#include <wfc/pubsub/api/notify.hpp>
//#include <wfc/callback/callback_status.hpp>

#include <functional>
#include <memory>

namespace wfc{ namespace pubsub{

struct ipubsub
{
  virtual ~ipubsub(){};

  ///
  /// request/response
  /// 
  typedef std::unique_ptr<request::publish>    request_publish_ptr;
  typedef std::unique_ptr<response::publish>   response_publish_ptr;

  typedef std::unique_ptr<request::multi_publish>    request_multi_publish_ptr;
  typedef std::unique_ptr<response::multi_publish>   response_multi_publish_ptr;

  typedef std::unique_ptr<request::load>    request_load_ptr;
  typedef std::unique_ptr<response::load>   response_load_ptr;

  typedef std::unique_ptr<request::multi_load>    request_multi_load_ptr;
  typedef std::unique_ptr<response::multi_load>   response_multi_load_ptr;

  typedef std::unique_ptr<request::subscribe>  request_subscribe_ptr;
  typedef std::unique_ptr<response::subscribe> response_subscribe_ptr;

  typedef std::unique_ptr<request::describe>  request_describe_ptr;
  typedef std::unique_ptr<response::describe> response_describe_ptr;

  
  typedef std::unique_ptr<request::query>  request_query_ptr;
  typedef std::unique_ptr<response::query> response_query_ptr;

  typedef std::unique_ptr<request::notify>  request_notify_ptr;
  typedef std::unique_ptr<response::notify> response_notify_ptr;

  ///
  /// callback functions
  ///

  typedef std::function< void(response_subscribe_ptr) > subscribe_callback;
  typedef std::function< void(response_describe_ptr) > describe_callback;
  typedef std::function< void(response_publish_ptr) > publish_callback;
  typedef std::function< void(response_multi_publish_ptr) > multi_publish_callback;
  typedef std::function< void(response_load_ptr) > load_callback;
  typedef std::function< void(response_multi_load_ptr) > multi_load_callback;
  typedef std::function< void(response_query_ptr) > query_callback;
  typedef std::function< void(response_notify_ptr) > notify_callback;
  
  typedef std::function< void(request_publish_ptr, publish_callback) > publish_handler;

  ///
  /// interface
  ///
  virtual void describe( size_t subscriber_id ) = 0;
  
  virtual void subscribe(request_subscribe_ptr, subscribe_callback, size_t subscriber_id, publish_handler ) = 0;
  
  virtual void describe(request_describe_ptr, describe_callback, size_t subscriber_id) = 0;

  virtual void publish(request_publish_ptr, publish_callback) = 0;

  virtual void publish(request_multi_publish_ptr, multi_publish_callback) = 0;
  
  virtual void load(request_load_ptr, load_callback ) = 0;

  virtual void load(request_multi_load_ptr, multi_load_callback ) = 0;

  virtual void query(request_query_ptr, query_callback ) = 0;
  
  virtual void notify(request_notify_ptr, notify_callback ) = 0;
};

/*
class pubsub: public ipubsub
{
  // TODO:
  // дефолтная имплементация ipubsub status::not_supported
};
*/

}}
