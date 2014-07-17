#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <wfc/logger.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{

struct ad_async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    if ( !t.status() )
      return;
    
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    auto pthis = t.shared_from_this();
    
    auto callback = [pthis, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
    { 
      typename T::lock_guard lk(pthis->mutex());
      pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(*dd), std::move(ec), bytes_transferred);
    };
    
    t.mutex().unlock();
    t.descriptor().async_read_some( ::boost::asio::buffer( **dd ), callback);
    t.mutex().lock();
  }
};

}}}
