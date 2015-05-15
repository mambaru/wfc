#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/connection/send/tags.hpp>

namespace wfc{ namespace io{ namespace connection{ namespace send{

  /*
struct ad_send
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    t.dispatch([&t, dd]()
    {
      auto d = t.get_aspect().template get<_make_buffer_>()(t, std::move(*dd) );
    
      if ( d != nullptr )
        t.get_aspect().template get< _outgoing_ >()(t, std::move(d) );
    }
  }

};
*/

}}}}
