#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/load.hpp>
#include <vector>

namespace wfc{ namespace pubsub{

namespace request
{
  struct multi_load_json
  {
    typedef ::wfc::json::array< std::vector< load_json::type > > type;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct multi_publish_json
  {
    typedef ::wfc::json::array< std::vector< publish_json::type > > type;
    typedef type::serializer serializer;
  };
}

}}
