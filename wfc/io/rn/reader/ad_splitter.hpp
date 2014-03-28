#pragma once

#include <wfc/io/rn/reader/tags.hpp>
#include <wfc/memory.hpp>
#include <vector>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

struct ad_splitter
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    typedef typename T::data_type data_type;
    auto& buffer = t.get_aspect().template get<_buffer_>();
    
    auto beg = d->begin();
    auto end = d->end();
    
    // если buffer != nullptr, то beg и end итераторы buffer
    // если buffer == nullptr, то beg и end итераторы d

    if ( buffer != nullptr )
    {
      buffer->reserve( buffer->size() + d->size() );
      std::copy(beg, end, std::back_inserter(*buffer));
      beg = buffer->begin();
      end = buffer->end();
    }

    while ( beg!=end )
    {
      auto itr = this->_parse(t, beg, end);
      
      // Если ничего не нашли
      if ( itr == beg )
      {
        // Если это первый "кусок"
        if ( buffer == nullptr )
        {
          buffer = std::move(d);
        }
        // Если часть буфера отправленно 
        else if (buffer->begin()!=beg)
        {
          buffer->erase(buffer->begin(), beg);
        }
        return this->check(t);
      }
      
      // Дошли до конца
      else if ( itr == end )
      {
        // buffer пустой, отправляем что пришло как есть
        if ( buffer == nullptr )
        {
          t.get_aspect().template get<_trimmer_>()(t, std::move(d) );
        }
        // buffer собрался один пакет целиком, отправляем его
        else if (buffer->begin() == beg )
        {
          t.get_aspect().template get<_trimmer_>()(t, std::move(buffer) );
          if ( buffer!=nullptr )
          {
            
            abort();
          }
        }
        // Отправляем что осталось
        else
        {
          t.get_aspect().template get<_trimmer_>()(t, std::make_unique<data_type>(beg, end) );
        }
        buffer = nullptr;
        return this->check(t);
      }
      // Очередной пакет
      else
      { 
        // Первый блок
        if ( buffer == nullptr )
        {
          // запоминаем хвост
          buffer = std::make_unique<data_type>(itr, end);
          // отправляем голову
          d->resize( std::distance(beg, itr) );
          t.get_aspect().template get<_trimmer_>()(t, std::move(d) );
          beg = buffer->begin();
          end = buffer->end();
        }
        else
        {
          t.get_aspect().template get<_trimmer_>()(t, std::make_unique<data_type>(beg, itr) );
          beg = itr;
        }
      }
    }
    
    return this->check(t);
    
  }
  
private:
  template<typename T>
  void check(T& t)
  {
    t.get_aspect().template get<_check_>()(t);
  }
  
  
  template<typename T, typename Itr>
  Itr _parse(T& t, Itr beg, Itr end)
  {
    static const char* sep =  t.get_aspect().template get<_separator_>()();
    
    Itr itr = beg;
    for(;itr!=end; ++itr)
    {
      if (*itr==*sep)
      {
        int i=1;
        for ( ;sep[i]!='\0' && (itr+i!=end) &&  *(itr+i)==sep[i]; ++i);
        if ( sep[i]=='\0' )
          return itr+i;
      }
    }
    // Не нашли
    return beg;
  }
private:
  // data_ptr buffer;
};

}}}}
