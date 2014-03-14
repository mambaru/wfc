//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/pubsub/api/publish_json.hpp>
#include <wfc/pubsub/api/subscribe_json.hpp>

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

JSONRPC_TAG(publish)  


struct method_list: wfc::jsonrpc::method_list<

  ::wfc::jsonrpc::target<ipubsub>,
  
  ::wfc::jsonrpc::method<
    ::wfc::jsonrpc::name<_publish_>,
    ::wfc::jsonrpc::invoke_mem_fun< 
        request::publish_json::type,
        response::publish_json::type,
        ipubsub, 
        &ipubsub::publish
    >
  >
  
  //,
  //::wfc::jsonrpc::interface_<ipubsub>,
  //::wfc::jsonrpc::shutdown< ::wfc::jsonrpc::mem_fun_ctrl<ipubsub, ipubsub, &ipubsub::describe> >

  //wfc::jsonrpc::interface_target_ctl<ipubsub, ipubsub, &itest_ex::startup, &itest_ex::shutdown>, 
/*
  wfc::inet::context<context>,
  jsonrpc::method< _publish_,   publish_method   >,
  jsonrpc::method< _subscribe_, subscribe_method >
  */
  
  
>{};

}}


