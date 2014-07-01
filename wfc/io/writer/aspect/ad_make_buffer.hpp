#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <iterator>
#include <memory>

namespace wfc{ namespace io{ namespace writer{
  
struct ad_make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t, typename T::data_ptr d)
  {
    typedef typename T::data_type data_type;
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    
    if ( lst.empty() )
    {
      if (d==nullptr)
        return nullptr;
      lst.push_back(nullptr);
      return std::move(d);
    }
    
    if ( d != nullptr )
    {
      size_t limit = t.options().output_buffer_size;
      
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
    
    if ( lst.empty() )
      return nullptr;
    
    return std::move( lst.front() );
  }
};

}}}
