#pragma once

#include <comet/pubsub/pubsub_status.hpp>
#include <comet/pubsub/api/publish.hpp>
#include <vector>

namespace mamba{ namespace comet{

namespace request
{
  typedef std::vector<publish> multi_publish;
  /*
  struct multi_publish
    : std::vector<publish>
  {
    
  };
  */
}

namespace response
{
  typedef std::vector<pubsub_status> multi_publish;
  /*struct multi_publish
  {

  };
  */
}

}}