//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <memory>
#include <wfc/pubsub/ipubsub.hpp>

namespace wfc{ namespace pubsub{

struct context
{
  std::weak_ptr< ipubsub > target;
};

}}

