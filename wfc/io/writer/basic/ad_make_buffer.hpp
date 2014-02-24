#pragma once

#include <wfc/io/writer/basic/tags.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{

struct ad_make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t, typename T::data_ptr d)
  {
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    
    if ( lst.empty() )
      return std::move(d);
    
    if ( d != nullptr )
    {
      size_t limit = t.get_aspect().template get<_output_buffer_size_>();
      
      auto beg = d->begin();
      auto end = d->end();
    
      for(;beg != end; )
      {
        if ( std::distance(beg, end) > static_cast<ptrdiff_t>(limit*2) ) // Последний блок может быть [BufferSize..BufferSize*2]
        {
          lst.push_back( std::make_unique<data_type>(beg, beg + limit));
          std::advance(beg, limit);
        }
        else
        {
          lst.push_back( std::make_unique<data_type>(beg, end) );
          beg = end;
        }
      }
    }

    return std::make_unique<typename T::data_type>( t.get_aspect().template get<_input_buffer_size_>() );
  }
};

}}}}
