//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once


#include <comet/pubsub/message.hpp>
#include <deque>
#include <algorithm>
#include <memory>

namespace mamba{ namespace comet{


/**
 * @brief Очередь сообщений message 
 *
 * Хранимые сообщения отсортированы по полю cursor.
 * Сообщения с одинаковым cursor распологаются в очереди в порядке поступления.
 * Перед выполнением большинства методов удаляются устаревшие сообщения
 *
 * Важно! Размер очереди сообщений управляется входящими сообщениям (атрибут limit). Если
 * размер очереди болше limit входящего сообщения, то удаляются наиболее старые
 *
 * Для опреции publish cообщения с нулевым временем жизни ( lifetime == 0 ) в очередь не помещаются
 * Сообщения с limit равным 0 также не помещаются в очередь, при этом сама очередь очищается
 * 
 * При помещение сообщения в очередь, его поле birthtime заменяется текущим временем.
 * Это поле не должно использоваться клиентом, т.к. это время не создания сообщения,
 * а помещения его в эту очередь. За время пути сообщение может пройти через несколько 
 * очередей или не попасть ни в одну.
 *
 * Идеальный вариант размещения сообщений в очереди:
 *   a) все сообщения очереди имеют одинаковый или возрастающий cursor
 *   б) все сообщения очереди имеют одинаковое время жизни lifetime
 *   в) все сообщения очереди имеют возрастающий birthtime *
 *                * (последовательность может быть нарушена командой update)
 *
 * В этом случай очередь считается упорядоченной по времени, и операция удаления
 * устаревших сообщений происходит быстрее.
 *
 * Очередь вновь будет считается упорядоченной при удалнии сообщений нарушающих эти условия
 * (проверка присходит при удалении устаревших сообщений)
 */
class message_queue
{
public:
  typedef std::unique_ptr<message> message_ptr;
  typedef std::deque<message_ptr> queue_type;

  struct f_cursor_less
{
  bool operator()(const message_ptr& left, const message_ptr& right) const
  {
    return left->cursor < right->cursor;
  }

  bool operator()(cursor_t left, const message_ptr& right) const
  {
    return left < right->cursor;
  }

  bool operator()(const message_ptr& left, cursor_t right) const
  {
    return left->cursor < right;
  }
};

public:
  message_queue();

  size_t size() const;
  
  bool empty() const;

  static bool is_not_for_empty_queue(const message& m);
  
  void clear();

  /** @brief Добавить сообщение, удалить или обновить сообщения
   *  @param m входящее сообщение
   *
   *  В зависимости от поля message::action сообщения, добавляет, удаляет или обновляет сообщения в канале:
   *    publish - добавляет сообщение в очередь
   *    modify  - обновляет контент сообщений и время жизни.
   *    update  - обновляет контент сообщений и время жизни, поле birthtime устанавливается в текущее время
   *    replace - заменяет сообщения (комбинация remove и publish)
   *    remove  - удаляет сообщения
   *
   * Опреации modify, update, replace и remove исползуют поле identity для идентификации сообщений.
   * При выполнении каждой из перечисленных операций происходит проход по всей очереди сообщений и 
   * сверяется аттрибут identity.
   * 
   * Для оперции remove достаточно правильно указать поле identity
   *
   * TODO:??? доставлять ли их клиенту, если доставлять то как?
   */
  void push( message_ptr m);
  

  /// @brief Добавляет сообщение в канал
  /** @return текущее количество сообщений в канале
   *
   * После добавления вызывает функцию remove_death
   * return nullptr - если остался в канале
   */
  message_ptr push( time_t now, message_ptr m);
  void resize(size_t limit);

  /// @brief Удаляет сообщения время жизни которых закончилось
  /** @return количество удаленных сообщений*/
  void remove_death();

  /// @brief Удаляет сообщения время жизни которых закончилось относительно заданного времени
  /**
   * @param now время относительно которого будет происходить проверка 
   * @return количество удаленных сообщений
  */
  void remove_death(time_t now);

  bool sequenced() const ;

  // не забываем делать remove_death() перед копированием 
  template<typename Itr>
  void copy_to(Itr itr)
  {
    /*
    for (const auto& m : _queue)
      *itr = message_ptr(new message(*m));
    */
    std::transform(_queue.begin(), _queue.end(), itr, [](const message_ptr& ptr){
      return message_ptr(new message(*ptr));
    });
    // std::copy(_queue.begin(), _queue.end(), itr);
  }

  // не забываем делать remove_death() перед копированием 
  template<typename Itr>
  void copy_to(cursor_t cursor, Itr itr) const
  {
    queue_type::const_iterator beg = std::lower_bound(_queue.begin(), _queue.end(), cursor, f_cursor_less() );
    std::transform(beg, _queue.end(), itr, [](const message_ptr& ptr){
      return message_ptr(new message(*ptr));
    });
  }

  // не забываем делать remove_death() перед копированием 
  template<typename Itr>
  void copy_to(cursor_t cursor, size_t limit, Itr itr) const
  {
    queue_type::const_iterator beg = std::lower_bound(_queue.begin(), _queue.end(), cursor, f_cursor_less() );
    queue_type::const_iterator end = _queue.end();
    size_t s = std::distance(beg, end);
    if ( limit < s )
      std::advance( beg, s - limit);
    std::transform(beg, end, itr, [](const message_ptr& ptr){
      return message_ptr(new message(*ptr));
    });

  }

private:

  queue_type::iterator _insert( time_t now, message_ptr m );
  
  /**
   * @brief добавляет сообщение в канал, в позицию cursor.
   * @param now текущее время
   * @param m публикуемое сообщение
   *
   * В канале сообщения хранятся в порядке возрастания курсора.
   * Если в канале уже существует сообщение(я) с указаным в m курсором,
   * то m добавляется за ним(и)
   */
  void _publish(time_t now, message_ptr m);

  /// @brief обновляет все сообшения с обновлением времени создания
  void _update(time_t now, message_ptr m);

  /// @brief обновляет все сообшения без обновления времени создания
  void _modify(time_t now, message_ptr m );

  /// @brief Удаляет все сообщения по identity
  //void _remove( time_t now, message_ptr m, bool publish_flag /*= true*/ );
  void _remove( time_t now, message_ptr m);
  void _remove( time_t now, const message& m);

  /// @brief
  void _replace(time_t now, message_ptr m );
  
private:
  queue_type _queue;
  
  /// флаг того, что время жизни всех сообщений канала упорядоченно 
  bool _sequenced_flag;
  time_t _last_remove_death;

};

}}


