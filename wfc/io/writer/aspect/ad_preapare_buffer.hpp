#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <wfc/memory.hpp>
#include <iterator>

namespace wfc{ namespace io{ namespace writer{
  
struct ad_preapare_buffer
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, std::size_t bytes_transferred)
  {
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    lst.pop_front();
    if ( d->size() != bytes_transferred )
    {
      this->preapare(t, std::move(d), bytes_transferred);
    }
  }

private:
  
  template<typename T>
  void preapare(T& t, typename T::data_ptr d, std::size_t bytes_transferred)
  {
    typedef typename T::data_type data_type;

    size_t size = d->size() - bytes_transferred;
    size_t limit = t.options().output_buffer_size;
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    
    auto beg = d->rbegin();
    auto end = d->rbegin() + size;

    for(;beg != end; )
    {
      if ( std::distance(beg, end) > static_cast<ptrdiff_t>(limit*2) ) // Последний блок может быть [BufferSize..BufferSize*2]
      {
        lst.push_front( std::make_unique<data_type>( (beg + limit).base(), beg.base()));
        std::advance(beg, limit);
      }
      else
      {
        lst.push_front( std::make_unique<data_type>(end.base(), beg.base() ) );
        beg = end;
      }
    }
  }
};

}}}
