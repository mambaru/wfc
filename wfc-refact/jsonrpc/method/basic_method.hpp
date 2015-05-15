#pragma once

#include <wfc/jsonrpc/method/method_base.hpp>
#include <wfc/jsonrpc/method/aspect_method.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{
  
template< typename... Args >
class basic_method
  : public method_base< typename fas::merge_aspect<fas::aspect<Args...>, aspect_method>::type >
{
public:
};

}}
