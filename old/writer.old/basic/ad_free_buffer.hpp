#pragma once

#include <wfc/io/writer/basic/tags.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{

/*
struct ad_free_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t, typename T::data_ptr d)
  {
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    lst.pop_front();
    return std::move(d);
  }
};
*/ 

}}}}
