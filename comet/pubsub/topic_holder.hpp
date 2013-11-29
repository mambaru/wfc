//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011, 2013
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <comet/pubsub/types.hpp>
#include <comet/pubsub/message_queue.hpp>
#include <stdexcept>


namespace mamba{ namespace comet{

/** @brief По сути обертка для message_queue */
class topic_holder
{
public:
  virtual ~topic_holder()
  {
    delete _message_queue;
  }

  topic_holder()
    : _message_queue(0)
  {
  }

  operator bool () const 
  {
    return !_name.empty();
  }

  explicit topic_holder(const std::string& name)
    : _message_queue(0)
    , _name(name)
  {
  }

  explicit topic_holder(const topic_holder& c)
    : _message_queue(c._message_queue)
    , _name(c._name)
  {
    const_cast<topic_holder&>(c)._message_queue = 0;
    const_cast<topic_holder&>(c)._name.clear();
  }

  void operator = (const topic_holder& c)
  {
    _message_queue = c._message_queue;
    _name = c._name;
    const_cast<topic_holder&>(c)._message_queue = 0;
    const_cast<topic_holder&>(c)._name.clear();
  }

  message_queue* operator -> () const
  {
    if ( !*this)
      throw std::logic_error("unnamed channel");
    
    if ( _message_queue == 0 )
      _message_queue = new message_queue;
    return _message_queue;
  }

  const std::string& name() const 
  {
    return _name;
  }

private:

  friend bool operator < (const topic_holder& first, const topic_holder& second);

private:
  mutable message_queue* _message_queue;
  std::string _name;
};

inline bool operator < (const topic_holder& first, const topic_holder& second)
{
  return first._name < second._name;
}

}}

