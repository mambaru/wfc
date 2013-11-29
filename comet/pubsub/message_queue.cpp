//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include <deque>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <comet/pubsub/types.hpp>
#include <comet/pubsub/message.hpp>
#include <comet/pubsub/message_queue.hpp>

#include <deque>
#include <algorithm>
#include <iostream>


namespace mamba{ namespace comet{

struct f_deathtime_less
{
  time_t now;

  f_deathtime_less(time_t now): now(now) {}

  bool operator()( const message_queue::message_ptr& m) const
  {
    return m->deathtime() < now;
  }
};

struct f_identity_equal
{
  const message& m;

  f_identity_equal(const message& m)
    : m(m)
  {
  }

  bool operator()( const message_queue::message_ptr& msg) const
  {
    return msg->identity == m.identity;
  }
};

struct f_modify_if_identity_equal
{
  const message& m;
  bool possible_violation_of_sequence;
  int count;
  f_modify_if_identity_equal(const message& m)
    : m(m)
    , possible_violation_of_sequence(false)
    , count(0)
  {
  }

  void operator() (message_queue::message_ptr& msg)
  {
    if (msg->identity == m.identity )
    {
      if ( !possible_violation_of_sequence && msg->limit != m.limit )
        possible_violation_of_sequence = true;

      msg->modify(m);
      ++count;
    }
  }
};

struct f_update_if_identity_equal
{
  time_t now;
  const message& m;
  int count;

  f_update_if_identity_equal( time_t now, const message& m )
    : now(now)
    , m(m)
    , count(0)
  {
    
  }

  void operator() (message_queue::message_ptr& msg)
  {
    if (msg->identity == m.identity )
    {
      msg->update(m);
      ++count;
    }
  }

};

/*
struct f_cursor_less
{
  bool operator()(const message_ptr& left, const message_ptr& right) const
  {
    return left.cursor < right.cursor;
  }

  bool operator()(cursor_t left, const message_ptr& right) const
  {
    return left < right.cursor;
  }

  bool operator()(const message_ptr& left, cursor_t right) const
  {
    return left.cursor < right;
  }
};
*/

message_queue::message_queue()
  : _sequenced_flag(true)
  , _last_remove_death(0)
{}

size_t message_queue::size() const
{
  return _queue.size();
}

bool message_queue::empty() const
{
  return _queue.empty();
}

void message_queue::clear()
{
  _queue.clear();
}

void message_queue::push( message_ptr m)
{
  this->push(time(0), std::move(m) );
}


bool message_queue::is_not_for_empty_queue(const message& m)
{
  return m.lifetime==0 || m.limit == 0;
}

message_queue::message_ptr message_queue::push( time_t now, message_ptr m)
{
  /** Сначала удаляем устаревшие сообщения */
  this->remove_death(now);

  if ( m->action == actions::publish )
  {
    /** Размер очереди установлен в 0 - сообщение в очередь не ставиться, а сразу доставляется клиенту, очередь очищается */
    if ( m->limit == 0)
      this->resize(0);

    /** Время жизни 0 - сообщение в очередь не ставиться, а сразу доставляется клиенту */
    if ( m->lifetime==0 || m->limit == 0 )
      return m;
  }

  size_t limit = m->limit;
  switch( m->action )
  {
    case actions::publish: _publish(now, std::move(m) ); break;
    case actions::modify:  _modify(now, std::move(m) ); break;
    case actions::update:  _update(now, std::move(m) ); break;
    case actions::replace: _replace(now, std::move(m) ); break;
    case actions::remove:  _remove(now, std::move(m) ); break;
    default: throw std::logic_error("message_queue::push: unknown method");
  }

  /** удаляем наиболее старые сообщения, даже если время жизни их не истекло */
  this->resize(limit);
  return nullptr;
}


void message_queue::resize(size_t limit)
{
  if ( _queue.size() <= limit)
    return;
  queue_type::iterator itr = _queue.begin();
  std::advance( itr, _queue.size() - limit);
  _queue.erase(_queue.begin(), itr);
}

void message_queue::remove_death()
{
  remove_death(time(0));
}

/// @brief Удаляет сообщения время жизни которых закончилось относительно заданного времени
  /**
   * @param now время относительно которого будет происходить проверка
   * @return количество удаленных сообщений
  */
void message_queue::remove_death(time_t now)
{
  if ( _last_remove_death == now)
    return;

  _last_remove_death = now;

  f_deathtime_less fdp(now);

  bool has_removed = false;

  // Сначала удаляем устаревшие из начала очереди
  while ( !_queue.empty() )
  {
    if ( fdp(_queue.front()) )
    {
      _queue.pop_front();
    }
    else
       break;
    has_removed = true;
  }

  // Если последовательность времен жизни нарушена
  if ( !_sequenced_flag && !_queue.empty())
  {
    size_t size = _queue.size();
    // Пробегаем по всей очереди и удаляем старые
    _queue.erase( std::remove_if(_queue.begin(), _queue.end(), fdp), _queue.end());
    has_removed |=  (size != _queue.size());
    if ( has_removed )
    {
      // Определяем восстановилась ли упорядоченность
      if ( _queue.size() > 1)
      {
        queue_type::iterator beg = _queue.begin();
        queue_type::iterator end = _queue.end();
        --end;
        for ( _sequenced_flag = true;  _sequenced_flag && beg!=end; ++beg )
        {
          queue_type::iterator itr = beg + 1;
          _sequenced_flag = ( (*beg)->lifetime == (*itr)->lifetime ) && ( (*beg)->birthtime <= (*itr)->birthtime );
        }
      }
      else
        _sequenced_flag = true;
    }
  }
}

bool message_queue::sequenced() const
{
  return _sequenced_flag;
}

message_queue::queue_type::iterator message_queue::_insert( time_t now, message_ptr m )
{
  // Находим позицию
  queue_type::iterator itr = std::upper_bound(_queue.begin(), _queue.end(), m, f_cursor_less() );

  // Вставляем
  itr = _queue.insert(itr, std::move(m) );
  if (itr==_queue.end())
    return itr;

  (*itr)->birthtime = now;

  return itr;
}

void message_queue::_publish(time_t now, message_ptr m)
{
  queue_type::iterator itr = _insert(now, std::move(m) );
  if ( itr==_queue.end() )
    return;
  queue_type::iterator mitr = itr;

// Проверяем упороядоченность сообщений по времени жизни
  // (для ускорения удаления)
  if ( _sequenced_flag )
  {
    if ( itr!=_queue.begin() )
    {
      --itr;
      _sequenced_flag = ( (*itr)->lifetime == (*mitr)->lifetime ) && ( (*itr)->birthtime <= now );
      ++itr;
    }
    ++itr;
    if ( _sequenced_flag && itr!=_queue.end())
    {
      _sequenced_flag = ( (*itr)->lifetime == (*mitr)->lifetime ) && ( (*itr)->birthtime >= now );
    }
  }
}

/// @brief обновляет все сообшения с обновлением времени создания
void message_queue::_update(time_t now, message_ptr m)
{
  f_update_if_identity_equal f = std::for_each(_queue.begin(), _queue.end(), f_update_if_identity_equal(now, *m) );
  if ( f.count > 0 )
    _sequenced_flag = false;
  if ( m->lifetime!=0 && f.count == 0 )
    _publish( now, std::move(m) );
}

/// @brief обновляет все сообшения без обновления времени создания
void message_queue::_modify(time_t now, message_ptr m )
{
  f_modify_if_identity_equal f = std::for_each(_queue.begin(), _queue.end(), f_modify_if_identity_equal(*m) );

  if ( _sequenced_flag && f.possible_violation_of_sequence )
    _sequenced_flag = false;

  if ( m->lifetime!=0 && f.count == 0)
    _publish( now, std::move(m) );
}

/// @brief Удаляет все сообщения по identity
void message_queue::_remove( time_t now, message_ptr m)
{
  this->_remove(now, *m);
  
  if ( m->lifetime!=0 )
    _publish( now, std::move(m) );
}

void message_queue::_remove( time_t now, const message& m)
{
  _queue.erase(
    std::remove_if(
      _queue.begin(),
      _queue.end(),
      f_identity_equal(m)
    ),
    _queue.end()
  );
  
}

/// @brief
void message_queue::_replace(time_t now, message_ptr m )
{
  _remove(now, *m);
  _publish(now, std::move(m) );
}

}}


