#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace reader{

struct ad_async_read
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    if ( t.get_aspect().template get<_callback_list_>().empty() )
    {
      t.get_aspect().template get<_incoming_list_>().push( std::move(d) );
    }
    else
    {
      auto c = t.get_aspect().template get<_callback_list_>().front();
      t.get_aspect().template get<_callback_list_>().pop();
      c( std::move(d) );
    }
  }
  
  template<typename T, typename Callback>
  void /*operator()*/call(T& t, Callback callback)
  {
    if( t.get_aspect().template get<_incoming_list_>().empty() )
    {
      std::cout << "-1-" << std::endl;
      t.get_aspect().template get<_callback_list_>().push(callback);
      t.get_aspect().template get<_async_read_some_>()(t);
    }
    else
    {
      std::cout << "-2-" << std::endl;
      typename T::data_ptr d = std::move( t.get_aspect().template get<_incoming_list_>().front() );
      t.get_aspect().template get<_incoming_list_>().pop();
      callback( std::move(d) );
    }
    /*
    typename T::data_ptr d = std::move( t.get_aspect().template get<_incoming_list_>().front() );
    t.get_aspect().template get<_incoming_list_>().pop();
    return std::move(d);
      */
  }

  
private:
  
  
};

}}}
