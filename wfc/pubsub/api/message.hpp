//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011, 2013, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/pubsub/api/types.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace pubsub{

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
  actions action /*= actions::publish*/;

  size_t limit /*= 0*/;

  /** Время регистрации. устанавлявается сервером в момент поступления события */
  mutable time_t birthtime /*= 0*/;

  /** Время жизни в секундах или время смерти в unix timespan
    */
  time_t lifetime /*= 0*/;

  /** некоторое числовое значение, по которому может выгружаться информация из канала 
   *  (выгружаются все у которых cursor >= заданного значения ) 
   *  а также сортировка
   */
  cursor_t cursor /*= 1*/;

  /** Некоторый идентификатор по которому происходит обновление или удаление сообщения из канала. */
  identity_t identity /*= ""*/;

  /** контент сообщения. */
  data_ptr content /*= nullptr*/;

  /** ключ доступа для client-side*/
  key_t key /*= 0*/;
  
  message()    
    : action(actions::publish)
    , limit(0)
    , birthtime(0)
    , lifetime(0)
    , cursor(1)
    , identity("")
    , content(nullptr)
    , key("")
 {};

  message(const message& other)
    : action( other.action )
    , limit( other.limit )
    , birthtime( other.birthtime )
    , lifetime( other.lifetime )
    , cursor( other.cursor )
    , identity( other.identity )
    , content( nullptr )
    , key( other.key )
  {
    this->update_content(other);
  }

  
  message(message&& other)
    : action( std::move(other.action) )
    , limit( std::move(other.limit) )
    , birthtime( std::move(other.birthtime) )
    , lifetime( std::move(other.lifetime) )
    , cursor( std::move(other.cursor) )
    , identity( std::move(other.identity) )
    , content( std::move(other.content) )
    , key( std::move(other.key) )
  {
  }
  
  message& operator = (message&& other)
  {
    this->action = std::move(other.action);
    this->limit = std::move(other.limit);
    this->birthtime = std::move(other.birthtime);
    this->lifetime = std::move(other.lifetime);
    this->cursor = std::move(other.cursor);
    this->identity = std::move(other.identity);
    this->content = std::move(other.content);
    this->key = std::move(other.key);
    return *this;
  }

  message& operator = (const message& other)
  {
    this->action = other.action;
    this->limit = other.limit;
    this->birthtime = other.birthtime;
    this->lifetime = other.lifetime;
    this->cursor = other.cursor;
    this->identity = other.identity;
    this->update_content(other);
    this->key = other.key;
    return *this;
  }

  /*
  message()
    : action(actions::publish)
    , limit(0)
    , birthtime(0)
    , lifetime(0)
    , cursor(1)
    , identity("")
  {}
  */

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
    if ( content==nullptr )
      return std::string();
    return std::string(content->begin(), content->end());
  }

  void str(const std::string& s)
  {
    content = std::make_unique<data_t>(s.begin(), s.end());
    //content.assign(s.begin(), s.end());
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
    this->update_content( m );
    key = m.key;
  }

  void modify(const message& m)
  {
    limit = m.limit;
    // content = m.content;
    this->update_content( m );
    key = m.key;
  }
  
  void update_content(const message& m)
  {
    if ( m.content != nullptr )
    {
      if ( content == nullptr )
      {
        content = std::make_unique<data_t>( m.content->begin(), m.content->end() );
      }
      else
      {
        content->assign( m.content->begin(), m.content->end() );
      }
    }
    else
    {
      content.reset();
    }
    
  }

};

}}



