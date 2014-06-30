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
    auto pthis = t.shared_from_this();
    
    auto callback = [this, pthis, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
    { 
          typename T::lock_guard lk(pthis->mutex());
          {
            if ( !pthis->status() )
            {
              // TODO: trace в аспект
              return;
            }
            
            // TODO: в трасе аспект на _on_read_
            TRACE_LOG_MESSAGE( "READ [[" << std::string((*dd)->begin(), (*dd)->end() ) << "]] " << ec.message() )
            
            if ( !ec )
            {
              (*dd)->resize(bytes_transferred);
              pthis->get_aspect().template get<_read_ready_>()(*pthis, std::move(*dd));
            }
            else
            {
              pthis->get_aspect().template get<_read_error_>()(*pthis, std::move(*dd), ec);
            }
          } // lock guard
    };
    
    //auto wcallback = t.owner().wrap(callback);

    
    t.mutex().unlock();
    
    
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ), callback
      /*t.strand().wrap(*/
      //)
    );
    t.mutex().lock();
    
  }
};

}}}
