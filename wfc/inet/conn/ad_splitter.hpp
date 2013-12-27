#pragma once

#include <vector>
#include <memory>
#include <wfc/memory.hpp>

namespace wfc{ namespace inet{

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
      if (itr == beg)
      {
        if ( _data == nullptr )
          _data = std::move(d);
        else if (_data->begin()!=beg)
          _data->erase(_data->begin(), beg);
        return;
      }
      else if ( itr == end )
      {
        if ( _data == nullptr )
          t.get_aspect().template get<Tg>()(t, std::move(d) );
        else if (_data->begin() == beg )
          t.get_aspect().template get<Tg>()(t, std::move(_data) );
        else
          t.get_aspect().template get<Tg>()(t, data_ptr( new data_type(beg, itr) ) );
        _data = nullptr;
        return;
      }
      else
      { 
        if ( _data == nullptr )
        {
          _data = data_ptr( new data_type(itr, end) );
          d->resize( std::distance(beg, itr) );
          t.get_aspect().template get<Tg>()(t, std::move(d) );
          beg = _data->begin();
          end = _data->end();
        }
        else
        {
          t.get_aspect().template get<Tg>()(t, data_ptr( new data_type(beg, itr) ) );
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

}}
