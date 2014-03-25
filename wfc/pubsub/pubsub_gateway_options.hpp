//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/io/io_base.hpp>
#include <string>

namespace wfc{ namespace pubsub{
  
struct pubsub_gateway_options: 
  ::wfc::io::basic::options
{
  std::string pubsub_name;
  
  std::string incoming_target;
  std::string incoming_channel;
  
  std::string outgoing_target;
  std::string outgoing_channel;
};

}}



