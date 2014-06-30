#pragma once

#include <wfc/io/acceptor/async/tags.hpp>
#include <wfc/io/basic/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace io{ namespace acceptor{ namespace async{

struct ad_async_accept
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    DEBUG_LOG_BEGIN("ad_async_accept -1-" << t.get_id() );
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    auto pthis=t.shared_from_this();
    auto callback =         [pthis, dd]( boost::system::error_code ec )
        { 
          DEBUG_LOG_BEGIN("ad_async_accept init " << pthis->get_id() );
          typename T::lock_guard lk( pthis->mutex() );
          
          if ( !pthis->status() )
            return;

          if ( !pthis->descriptor().is_open() )
            return;
          
          DEBUG_LOG_BEGIN("ad_async_accept ready " << pthis->get_id() );
          pthis->get_aspect().template get< _outgoing_>()(*pthis, std::move(*dd), ec);
          DEBUG_LOG_END("ad_async_accept ready " << pthis->get_id() );
        };
    //auto wcallback = t.owner().wrap(callback);

    
    t.mutex().unlock();
    DEBUG_LOG_BEGIN("ad_async_accept -2- " << t.get_id() );
    t.descriptor().async_accept
    (
      **dd,
      callback
    );
    DEBUG_LOG_BEGIN("ad_async_accept -2- " << t.get_id() );
    t.mutex().lock();
    DEBUG_LOG_END("ad_async_accept -1- " << t.get_id() );
  }
};

}}}}
