#pragma once

#include <wfc/pubsub/pubsub_status.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <vector>

namespace wfc{

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

}