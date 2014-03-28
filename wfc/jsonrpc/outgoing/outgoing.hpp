#pragma once

#include <wfc/io/types.hpp>
#include <utility>
#include <wfc/json/json.hpp>
#include <fas/type_list.hpp>

namespace wfc{ namespace jsonrpc{

struct outgoing
{
  typedef char version_type[4];
  version_type version;
  outgoing()
    : version{'2','.','0','\0'}
  {}
};

}}
