//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011, 2013, 2014
//
// Copyright: See COPYING file that comes with this distribution
//


#pragma once

#include <wfc/pubsub/api/message.hpp>
#include <wfc/json/json.hpp>
#include <wfc/json/name.hpp>

namespace wfc{ namespace pubsub{

struct message_json
{

  FAS_NAME(publish)
  FAS_NAME(modify)
  FAS_NAME(update)
  FAS_NAME(replace)
  FAS_NAME(remove)
  FAS_NAME(action)
  FAS_NAME(limit)
  FAS_NAME(lifetime)
  FAS_NAME(cursor)
  FAS_NAME(identity)
  FAS_NAME(key)
  FAS_NAME(content)


  typedef fas::type_list_n<
    json::enum_value<n_publish, actions, actions::publish>,
    json::enum_value<n_modify, actions, actions::modify>,
    json::enum_value<n_update, actions, actions::update>,
    json::enum_value<n_replace, actions, actions::replace>,
    json::enum_value<n_remove, actions, actions::remove>
  >::type actions_enum_list;

  typedef
    json::object<
      message,
      fas::type_list_n<
        json::member<n_action, message, actions, &message::action,
                  json::enumerator< actions, actions_enum_list> >,
        json::member<n_limit, message, size_t, &message::limit >,
        json::member<n_lifetime, message, time_t, &message::lifetime >,
        json::member<n_cursor, message, cursor_t, &message::cursor >,
        json::member<n_identity, message, identity_t, &message::identity >,
        json::member<n_key, message, key_t, &message::key >,
        json::member<n_content, message, data_t, &message::content,  json::raw_value<data_t> >
      >::type
    > type;

  typedef type::serializer serializer;
  
  typedef std::unique_ptr<message> message_ptr;
  
  typedef json::pointer<message_ptr, type> pointer;
  
};



}}
