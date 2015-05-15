#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/logger.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace basic{ namespace log{

struct ad_create
{
  template<typename T,  typename Init>
  void operator()(T& t, const Init& init)
  {
    auto func = init.not_alive;
    t.get_aspect().template get<_not_alive_>() = [&t, func]()
    {
      if ( func )
        func();
      COMMON_LOG_WARNING( "wfc io object not alive (handler captured dead object)" )
    };
  }
};

}}}}
