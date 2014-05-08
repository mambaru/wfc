//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/pubsub/pubsub_gateway_options.hpp>

namespace wfc{ namespace pubsub{
  
struct pubsub_gateway_options_json
{
  JSON_NAME(pubsub_name)
  JSON_NAME(incoming_target)
  JSON_NAME(incoming_channel)
  JSON_NAME(outgoing_target)
  JSON_NAME(outgoing_channel)
  JSON_NAME(subscribe_suffix)

  typedef ::wfc::json::object<
    pubsub_gateway_options,
    fas::type_list_n<
      ::wfc::json::member< n_pubsub_name,      pubsub_gateway_options, std::string,  &pubsub_gateway_options::pubsub_name >,
      ::wfc::json::member< n_incoming_target,  pubsub_gateway_options, std::string,  &pubsub_gateway_options::incoming_target >,
      ::wfc::json::member< n_incoming_channel, pubsub_gateway_options, std::string,  &pubsub_gateway_options::incoming_channel >,
      ::wfc::json::member< n_outgoing_target,  pubsub_gateway_options, std::string,  &pubsub_gateway_options::outgoing_target >,
      ::wfc::json::member< n_outgoing_channel, pubsub_gateway_options, std::string,  &pubsub_gateway_options::outgoing_channel >,
      ::wfc::json::member< n_subscribe_suffix, pubsub_gateway_options, std::string,  &pubsub_gateway_options::subscribe_suffix >
    >::type
  > type;
};

}}



