#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>
#include <wfc/io/reader/tags.hpp>

namespace wfc{ namespace io{ namespace reader{

struct ad_read_some
{
  template<typename T>
  void operator()(T& t)
  {
    //typedef typename T::data_type data_type;
    //auto d = std::make_unique<data_type>(8096);
    //d->resize(s);
    
    auto d = t.get_aspect().template get<_create_buffer_>()(t);
    
    boost::system::error_code ec;
    
    size_t bytes_transferred = 
      t.get_aspect().template get<_descriptor_ptr_>()->read_some( ::boost::asio::buffer( *d ), ec);
      
    std::cout << "bytes_transferred " << bytes_transferred << std::endl;
    if (!ec)
    {
      d->resize(bytes_transferred);
      t.get_aspect().template get<_read_>()( t, std::move(d));
    }
    else if ( ec == boost::asio::error::operation_aborted)
    {
      t.get_aspect().template get<_aborted_>()(t);
    }
    else
    {
      t.get_aspect().template get<_error_>()(t, ec);
    }
  }
};

}}}
