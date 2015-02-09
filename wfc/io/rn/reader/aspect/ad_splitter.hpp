#pragma once

#include <wfc/io/rn/reader/aspect/tags.hpp>
#include <wfc/memory.hpp>
#include <vector>
#include <list>

namespace wfc{ namespace io{ namespace rn{ namespace reader{

#warning оптимизировать разбиение потока
// Если приходит мусор или очень большой запрос, то пишеться в конец буфера
// что не эффективно 
  
template<typename DataType>
class buffer
{
  typedef DataType data_type;
  typedef typename data_type::value_type value_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::list<data_ptr> data_list;
  typedef std::unique_ptr<data_list> list_ptr;
public:
  
  buffer(value_type* sep) // int, чтоб траблов с коструктором копирования небыло
  {
  }
  
  data_ptr parse(data_ptr d, value_type* sep)
  {
    // Не забыть проверку на разрыв sep
    if ( _list!=nullptr && _list[0]!=nullptr )
    {
      data_ptr& last = _list.back();
      if ( last.capacity() - last.size() > d->size() )
      {
        // переносим в последний
        // или если есть разрыв sep, независимо от размера
      }
      else
      {
        _list->push_back( std::move(d) );
      }
    }
    return std::move(d);
  }
  
private:  
  list_ptr _list;
};

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
