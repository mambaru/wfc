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
  JSON_NAME(publish)
  JSON_NAME(modify)
  JSON_NAME(update)
  JSON_NAME(replace)
  JSON_NAME(remove)
  JSON_NAME(action)
  JSON_NAME(limit)
  JSON_NAME(lifetime)
  JSON_NAME(cursor)
  JSON_NAME(identity)
  JSON_NAME(key)
  JSON_NAME(content)


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
        json::member<
          n_key,
          message,
          key_ptr,
          &message::key,
          json::pointer<
            key_ptr,
            json::raw_value<key_t> >
        >,
        json::member<
          n_content, 
          message, 
          data_ptr, 
          &message::content,  
          json::pointer<
            data_ptr,
            json::raw_value<data_t> 
          >
        >
      >::type
    > type;

  typedef type::serializer serializer;
  
  typedef std::unique_ptr<message> message_ptr;
  
  typedef json::pointer<message_ptr, type> pointer;
  
};



}}
