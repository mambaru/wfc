#pragma once

#include <wfc/jsonrpc/noparams.hpp>
#include <wfc/json.hpp>
#include <fas/type_list.hpp>

namespace wfc{ namespace jsonrpc{

struct noparams_json
{
  typedef ::wfc::json::object<
    noparams,
    fas::type_list_n<
    >::type
  > type;
  
  typedef type::serializer serializer;
  typedef type::target target;
};

}}