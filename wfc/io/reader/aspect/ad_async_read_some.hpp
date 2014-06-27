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
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    auto callback = [this, &t, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
    { 
          typename T::lock_guard lk(t.mutex());
          {
            if ( !t.status() )
            {
              // TODO: trace в аспект
              return;
            }
            
            // TODO: в трасе аспект на _on_read_
            TRACE_LOG_MESSAGE( "READ [[" << std::string((*dd)->begin(), (*dd)->end() ) << "]]" )
            
            if ( !ec )
            {
              (*dd)->resize(bytes_transferred);
              t.get_aspect().template get<_read_ready_>()(t, std::move(*dd));
            }
            else
            {
              t.get_aspect().template get<_read_error_>()(t, std::move(*dd), ec);
            }
          } // lock guard
    };
    
    auto wcallback = t.owner().wrap(callback);

    
    t.mutex().unlock();
    
    
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ), wcallback
      /*t.strand().wrap(*/
      //)
    );
    t.mutex().lock();
    
  }
};

}}}
