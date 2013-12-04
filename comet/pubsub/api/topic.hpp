//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011, 2013
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <comet/pubsub/api/message.hpp>
#include <stdexcept>
#include <memory>


namespace mamba{ namespace comet{

/** @brief По сути обертка для message_queue */
struct topic
{
  typedef std::unique_ptr<message> message_ptr;
  std::string channel;
  std::vector< message_ptr > messages;
};


}}

