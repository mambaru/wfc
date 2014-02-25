#pragma once

#include <wfc/io/writer/basic/tags.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace basic{

struct ad_make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t, typename T::data_ptr d)
  {
    typedef typename T::data_type data_type;
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    
    std::cout << "ad_make_buffer -1-" << (d==nullptr) << std::endl;
    if ( lst.empty() )
    {
      std::cout << "ad_make_buffer -2-" << (d==nullptr) << std::endl;
      if (d==nullptr)
        return nullptr;
      lst.push_back(nullptr);
      std::cout << "ad_make_buffer -3- " << (d==nullptr) << std::endl;
      return std::move(d);
    }
    
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
    
    if ( lst.empty() )
      return nullptr;
    
    // TODO: отдать в список буфферов
    
    d = std::move( lst.front() );
    
    // Оставляем до free_buffer
    // lst.pop_front();
    
    return std::move(d);
  }
};

}}}}
