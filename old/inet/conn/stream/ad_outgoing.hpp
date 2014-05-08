#pragma once

#include <wfc/inet/conn/stream/tags.hpp>
#include <wfc/inet/types.hpp>

namespace wfc{ namespace inet{ namespace conn{ namespace stream{

struct ad_outgoing
{
  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    t.get_aspect().template get<_writer_>()(t, std::move(d));
  }
};

}}}}
