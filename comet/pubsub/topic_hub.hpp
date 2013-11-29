//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011, 2013
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <comet/pubsub/types.hpp>
#include <comet/pubsub/topic.hpp>
#include <comet/pubsub/topic_holder.hpp>
#include <comet/pubsub/message_queue.hpp>

#include <set>
#include <map>
#include <stdexcept>
#include <iostream>


namespace mamba{ namespace comet{

/** @brief Канал */
class topic_hub
{
  //typedef 
  typedef std::map<std::string, topic_holder> topic_set;
  // typedef std::set<topic_holder> topic_set;

  typedef std::unique_ptr<message_queue> message_queue_ptr;
  typedef std::map<std::string, std::unique_ptr<message_queue> > topic_map;

public:
  typedef std::unique_ptr<message> message_ptr;
  typedef std::vector<message_ptr> message_vector;
  typedef std::unique_ptr<message_vector> message_vector_ptr;

  virtual ~topic_hub(){}
  topic_hub(){}
  topic_hub(const topic_hub& ) = delete;
  topic_hub& operator=(const topic_hub& ) = delete;

  message_ptr push( const std::string& name, message_ptr m, time_t now = time(0) )
  {
    auto itr = _topics.find( name );
    if ( itr == _topics.end() )
    {
      if ( message_queue::is_not_for_empty_queue(*m) )
        return m;

      itr = _topics.insert( std::make_pair(name, message_queue_ptr(new message_queue) ) ).first;
    }
    return itr->second->push( now, std::move(m) );
  }

  topic_map::iterator _find_and_remove_death( const std::string& name, time_t now )
  {
    topic_map::iterator itr = _topics.find( name );
    if ( itr == _topics.end() )
      return itr;
    itr->second->remove_death(now);
    if ( itr->second->empty() )
    {
      _topics.erase(itr);
      return _topics.end();
    }
    return itr;
  }

  message_vector_ptr get_messages( const std::string& name, time_t now = time(0))
  {
    auto itr = _find_and_remove_death(name, now);
    if (itr == _topics.end())
      return nullptr;

    message_vector_ptr messages = message_vector_ptr(new message_vector);
    itr->second->copy_to( std::back_inserter(*messages) );
    return messages;
  }

  message_vector_ptr get_messages( const std::string& name, cursor_t cursor, time_t now = time(0))
  {
    auto itr = _find_and_remove_death(name, now);
    if (itr == _topics.end())
      return nullptr;

    message_vector_ptr messages = message_vector_ptr(new message_vector);
    itr->second->copy_to( cursor, std::back_inserter(*messages) );
    return messages;
  }

  message_vector_ptr get_messages( const std::string& name, cursor_t cursor, size_t limit, time_t now = time(0))
  {
    auto itr = _find_and_remove_death(name, now);
    if (itr == _topics.end())
      return nullptr;

    message_vector_ptr messages = message_vector_ptr(new message_vector);
    itr->second->copy_to( cursor, limit, std::back_inserter(*messages) );
    return messages;
  }

  /*
  explicit topic_hub(const topic_hub& c)
  {
    const_cast<topic_hub&>(c)._topics.swap(_topics);
  }
  */

  /*
  const topic_holder& get(const std::string& name) const
  {
    if ( name.empty() )
      return _empty_channel;

    topic_set::const_iterator itr = _topics.find( topic_holder(name) );

    if ( itr==_topics.end() )
      return _empty_channel;

    return *itr;
  }

  const topic_holder& at(const std::string& name)
  {
    if ( name.empty() )
      return _empty_channel;

    topic_set::iterator itr = _topics.lower_bound( topic_holder(name) );
    if ( itr==_topics.end() || itr->name()!=name)
      itr = _topics.insert(itr, topic_holder(name) );
    return *itr;
  }

  const topic_holder& operator[](const std::string& name)
  {
    return this->at(name);
  }
  */

  void erase(const std::string& name)
  {
    _topics.erase( name );
  }

  //void takeaway(std::vector<message>& m)
  void takeaway(std::vector< std::unique_ptr<topic> >& m)
  {
#warning TODO
    /*
    if ( _topics.empty() )
      return;

    time_t now = time(0);
    topic_set::iterator itr = _topics.begin();
    for ( ;itr!=_topics.end(); ++itr)
    {
      m.reserve(m.size() + (*itr)->size());
      size_t offset = m.size();

      (*itr)->remove_death();
      (*itr)->copy_to( std::back_inserter(m));
      (*itr)->clear();

      if (offset < m.size() )
      {
        std::vector<message>::iterator mitr = m.begin() + offset;
        for ( ;mitr!=m.end();++mitr)
        {
          mitr->channel = itr->name();
          time_t lived = now - mitr->birthtime;
          mitr->lifetime = mitr->lifetime > lived ? mitr->lifetime - lived : 0;
        }
      }
    }
    */
  }

  void idle()
	{
    #warning TODO
    /*
		size_t limit = _topics.size()/100;
		if (limit==0) limit = 10;
		topic_set::iterator beg = _topics.lower_bound( topic_holder(_idle_channel) );
		topic_set::iterator end = _topics.end();
		for ( size_t i = 0; (beg!=end) && (i < limit); ++i)
		{
			if ( *beg )
			{
				(*beg)->remove_death();
				if ( (*beg)->empty() )
					_topics.erase(beg++);
				else
					beg++;
			}
			else
				_topics.erase(beg++);
		}

		if ( beg!=end && *beg)
			_idle_channel = beg->name();
		else
			_idle_channel.clear();
    */
	}

private:

  topic_map   _topics;
  topic_holder       _empty_channel;
	std::string _idle_channel;
};

}}



