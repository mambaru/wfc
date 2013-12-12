//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/pubsub/api/types.hpp>

namespace wfc{

enum class actions
{
  publish,
  modify,
  update,
  replace,
  remove
};

/** @brief Сообщение */
struct message
{
  actions action;

  size_t limit;

  /** Время регистрации. устанавлявается сервером в момент поступления события */
  mutable time_t birthtime;

  /** Время жизни в секундах или время смерти в unix timespan
    */
  time_t lifetime;

  /** некоторое числовое значение, по которому может выгружаться информация из канала 
   *  (выгружаются все у которых cursor >= заданного значения ) 
   *  а также сортировка
   */
  cursor_t cursor;

  /** Некоторый идентификатор по которому происходит обновление или удаление сообщения из канала. */
  identity_t identity;

  /** контент сообщения. */
  data_t content;

  /** ключ доступа для client-side*/
  key_t key;

  message()
    : action(actions::publish)
    , limit(0)
    , birthtime(0)
    , lifetime(0)
    , cursor(1)
    , identity("")
  {}

  /*
  message_base(size_t limit, time_t lifetime, cursor_t cursor, const identity_t& identity, actions::type action = actions::publish)
    : action(action)
    , limit(limit)
    , birthtime(time(0))
    , lifetime(lifetime)
    , cursor(cursor)
    , identity(identity)
  {
  }

  message_base(size_t limit, time_t lifetime, cursor_t cursor, const identity_t& identity, const data_t& content, actions::type action = actions::publish)
    : action(action)
    , limit(limit)
    , birthtime(time(0))
    , lifetime(lifetime)
    , cursor(cursor)
    , identity(identity)
    , content(content)
  {
  }

  message_base(size_t limit, time_t lifetime, cursor_t cursor, const identity_t& identity, const std::string& str, actions::type action = actions::publish)
    : action(action)
    , limit(limit)
    , birthtime(time(0))
    , lifetime(lifetime)
    , cursor(cursor)
    , identity(identity)
    , content(str.begin(), str.end() )
  {
  }

  message_base(size_t limit, time_t birthtime, time_t lifetime, cursor_t cursor, const identity_t& identity, actions::type action = actions::publish)
    : action(action)
    , limit(limit)
    , birthtime(birthtime)
    , lifetime(lifetime)
    , cursor(cursor)
    , identity(identity)
  {
  }

  message_base(size_t limit, time_t birthtime, time_t lifetime, cursor_t cursor, const identity_t& identity, const data_t& content, actions::type action = actions::publish)
    : action(action)
    , limit(limit)
    , birthtime(birthtime)
    , lifetime(lifetime)
    , cursor(cursor)
    , identity(identity)
    , content(content)
  {
  }

  message_base(size_t limit, time_t birthtime, time_t lifetime, cursor_t cursor, const identity_t& identity, const std::string& str, actions::type action = actions::publish)
    : action(action)
    , limit(limit)
    , birthtime(birthtime)
    , lifetime(lifetime)
    , cursor(cursor)
    , identity(identity)
    , content(str.begin(), str.end() )
  {
  }
  */

  std::string str() const
  {
    return std::string(content.begin(), content.end());
  }

  void str(const std::string& s)
  {
    content.assign(s.begin(), s.end());
  }

  time_t deathtime() const
  {
    // если в lifetime время жизни (колько живет в сек)
    return birthtime + lifetime;
  }

  void update(const message& m)
  {
    limit = m.limit;
    birthtime = m.birthtime;
    lifetime = m.lifetime;
    content = m.content;
    key = m.key;
  }

  void modify(const message& m)
  {
    limit = m.limit;
    content = m.content;
    key = m.key;
  }

};

}



