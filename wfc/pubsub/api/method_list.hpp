//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/pubsub/api/subscribe_json.hpp>
#include <wfc/pubsub/api/publish_json.hpp>
#include <wfc/pubsub/api/describe_json.hpp>
#include <wfc/pubsub/api/multi_publish_json.hpp>
#include <wfc/jsonrpc.hpp>

namespace wfc{ namespace pubsub{

/*
JSONRPC_TAG(test1)  
struct method_test2: wfc::jsonrpc::method<
  wfc::jsonrpc::name<_test2_>,
  wfc::jsonrpc::invoke_mem_fun<test1_json, test1_json, itest, &itest::test1 >, 
  wfc::jsonrpc::call<test1_json, test1_json>
>{};
JSONRPC_METHOD_IMPL( _test2_, test1 )
*/

/*


JSONRPC_TAG(publish)
JSONRPC_TAG(mpublish)  
JSONRPC_TAG(subscribe)  
JSONRPC_TAG(describe)  




struct basic_method_list: ::wfc::jsonrpc::method_list<

  //TODO: dual_interface
  ::wfc::jsonrpc::target<ipubsub>,
  ::wfc::jsonrpc::interface_<ipubsub>,
  ::wfc::jsonrpc::provider< ipubsub >,
  ::wfc::jsonrpc::shutdown< ::wfc::jsonrpc::mem_fun_shutdown< ipubsub, &ipubsub::describe> >, 
  
  ::wfc::jsonrpc::dual_method<
    _publish_, 
    request::publish_json::type,
    response::publish_json::type,
    ipubsub, 
    &ipubsub::publish
  >,

  ::wfc::jsonrpc::dual_method<
    _mpublish_, 
    request::multi_publish_json::type,
    response::multi_publish_json::type,

    ipubsub, 
    &ipubsub::publish
  >,
  
  ::wfc::jsonrpc::dual_method2<
    _subscribe_, 
    request::subscribe_json::type,
    response::subscribe_json::type,
    request::publish,
    response::publish,
    ipubsub, 
    &ipubsub::subscribe,
    ipubsub, 
    &ipubsub::publish
  >,
  

  ::wfc::jsonrpc::dual_method3<
    _describe_, 
    request::describe_json::type,
    response::describe_json::type,
    ipubsub, 
    &ipubsub::describe
  >
>
{};

struct method_list: basic_method_list
{
  typedef basic_method_list super;
  
  virtual void describe(size_t)
  {
  }

  
  virtual void subscribe(request_subscribe_ptr req, subscribe_callback callback, size_t, publish_handler )
  {
    
    {
      this->call<_subscribe_>( std::move(req), callback, nullptr  );
      
    
    }
  }
  
  virtual void publish(request_publish_ptr req, publish_callback callback)
  {
    {
      this->call<_publish_>( std::move(req), callback, nullptr );
    }
  }


  virtual void publish(request_multi_publish_ptr req, multi_publish_callback callback)
  {
    this->call<_mpublish_>( std::move(req), callback, nullptr );
  }

  
  
  virtual void describe(request_describe_ptr req, describe_callback callback, size_t )
  {
    this->call< ::wfc::pubsub::_describe_ >( 
      std::move( req ), 
      callback, 
      [callback]( super::error_ptr)
      {
        auto resp = std::make_unique< ::wfc::pubsub::response::describe >();
        resp->status = ::wfc::pubsub::status::bad_gateway;
        if ( callback!=nullptr )
          callback( std::move(resp));
      }
    );
  }

  
  //virtual void subscribe(request_subscribe_ptr, subscribe_callback, size_t , publish_handler ) {}
  //virtual void publish( ipubsub::request_multi_publish_ptr, ipubsub::multi_publish_callback) {}
  virtual void load( ipubsub::request_load_ptr, ipubsub::load_callback ) {}
  virtual void load( ipubsub::request_multi_load_ptr, ipubsub::multi_load_callback ) {}
  virtual void query( ipubsub::request_query_ptr, ipubsub::query_callback ) {}
  virtual void notify( ipubsub::request_notify_ptr, ipubsub::notify_callback ) {}
};
*/

}}


