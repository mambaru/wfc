#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <vector>

namespace wfc{ namespace pubsub{

namespace request
{
  typedef std::vector<publish> multi_publish;
}

namespace response
{
  typedef std::vector<publish> multi_publish;
}

}}