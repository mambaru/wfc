#pragma once

#include <wfc/pubsub/status.hpp>
#include <wfc/pubsub/api/publish.hpp>
#include <vector>

namespace wfc{ namespace pubsub{

namespace request
{
  //typedef std::vector< std::unique_ptr<publish> > multi_publish;
  
  struct multi_publish_json
  {
    typedef ::wfc::json::array<
      std::vector<
          publish_json::type
      >
    > type;
    typedef type::serializer serializer;
  };

}

namespace response
{
  struct multi_publish_json
  {
    typedef ::wfc::json::array<
      std::vector<
          publish_json::type
      >
    > type;
    typedef type::serializer serializer;
  };
}

}}