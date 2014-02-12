#pragma once

#include <vector>
#include <memory>
#include <wfc/memory.hpp>

namespace wfc{ namespace inet{ namespace rn{

template<typename Tg, typename Str>
struct ad_splitter
{
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type> data_ptr;

  template<typename T>
  void operator()(T& t, data_ptr d)
  {
    auto beg = d->begin();
    auto end = d->end();
    
    std::cout << "DGRAM SPLIT " << std::string( beg, end ) << std::endl;
    
    // если _data != nullptr, то beg и end итераторы _data
    // если _data == nullptr, то beg и end итераторы d

    if ( _data != nullptr )
    {
      _data->reserve( _data->size() + d->size() );
      std::copy(beg, end, std::back_inserter(*_data));
      beg = _data->begin();
      end = _data->end();
    }

    while ( beg!=end )
    {
      auto itr = _parse(beg, end);
      
      // Если ничего не нашли
      if ( itr == beg )
      {
        // Если это первый "кусок"
        if ( _data == nullptr )
        {
          _data = std::move(d);
        }
        // Если часть буфера отправленно 
        else if (_data->begin()!=beg)
        {
          _data->erase(_data->begin(), beg);
        }
        return;
      }
      
      // Дошли до конца
      else if ( itr == end )
      {
        // _data пустой, отправляем что пришло как есть
        if ( _data == nullptr )
        {
          t.get_aspect().template get<Tg>()(t, std::move(d) );
        }
        // _data собрался один пакет целиком, отправляем его
        else if (_data->begin() == beg )
        {
          t.get_aspect().template get<Tg>()(t, std::move(_data) );
          if ( _data!=nullptr )
          {
            std::cout << "ad_splitter abort" << std::endl;
            abort();
          }
        }
        // Отправляем что осталось
        else
        {
          t.get_aspect().template get<Tg>()(t, std::make_unique<data_type>(beg, end) );
        }
        _data = nullptr;
        return;
      }
      // Очередной пакет
      else
      { 
        // Первый блок
        if ( _data == nullptr )
        {
          // запоминаем хвост
          _data = std::make_unique<data_type>(itr, end);
          // отправляем голову
          d->resize( std::distance(beg, itr) );
          t.get_aspect().template get<Tg>()(t, std::move(d) );
          beg = _data->begin();
          end = _data->end();
        }
        else
        {
          t.get_aspect().template get<Tg>()(t, std::make_unique<data_type>(beg, itr) );
          beg = itr;
        }
      }
    }
    
  }
  
private:
  
  data_type::iterator _parse(data_type::iterator beg, data_type::iterator end)
  {
    data_type::iterator itr = beg;
    const char* sep =  Str()();
    
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
  data_ptr _data;
};

}}}
