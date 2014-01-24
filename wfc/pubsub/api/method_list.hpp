//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/pubsub/api/publish_method.hpp>
#include <wfc/pubsub/api/subscribe_method.hpp>
#include <wfc/pubsub/api/context.hpp>
#include <wfc/pubsub/api/tags.hpp>
#include <wfc/jsonrpc/method.hpp>
#include <wfc/inet/context.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace pubsub{

struct method_list: fas::aspect<
  wfc::inet::context<context>,
  jsonrpc::method< _publish_,   publish_method   >,
  jsonrpc::method< _subscribe_, subscribe_method >
  
  /*
  ,
  jsonrpc::method< _publish_, publish_method >, 
  jsonrpc::method< _subscribe_, subscribe_method >
  */
>{};

}}


