#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <wfc/logger.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{

template<typename Tg>
struct read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
   
    boost::system::error_code ec;
    
    size_t bytes_transferred = 
      t.descriptor().read_some( ::boost::asio::buffer( *d ), ec);

    if ( ec )
    {
      t.get_aspect().template get<_status_>() = false;
      t.get_aspect().template get<_error_code_>() = ec;
    }
      
    d->resize(bytes_transferred);
    t.get_aspect().template get< Tg >()(t, std::move(d));
    
  }
};

}}}
