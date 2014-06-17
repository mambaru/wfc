//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011, 2013, 2014
//
// Copyright: See COPYING file that comes with this distribution
//


#pragma once

#include <wfc/pubsub/api/topic.hpp>
#include <wfc/pubsub/api/message_json.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace pubsub{

struct topic_json
{
  FAS_NAME(channel)
  FAS_NAME(messages)

  typedef json::object<
    topic,
    fas::type_list_n<
      json::member<n_channel, topic, std::string, &topic::channel >,
      json::member<n_messages, topic, std::vector< topic::message_ptr >, &topic::messages, ::wfc::json::array<message_json::pointer> >
    >::type
  > type;

  typedef type::serializer serializer;
};

}}
