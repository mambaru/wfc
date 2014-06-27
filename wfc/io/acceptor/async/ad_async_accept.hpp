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
    
    t.mutex().unlock();
    DEBUG_LOG_BEGIN("ad_async_accept -2-" << t.get_id() );
    t.descriptor().async_accept
    (
      **dd,
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec )
        { 
          DEBUG_LOG_BEGIN("ad_async_accept init" << t.get_id() );
          typename T::lock_guard lk( t.mutex() );
          
          if ( !t.descriptor().is_open() )
            return;
          
          DEBUG_LOG_BEGIN("ad_async_accept ready" << t.get_id() );
          t.get_aspect().template get< _outgoing_>()(t, std::move(*dd), ec);
          DEBUG_LOG_END("ad_async_accept ready" << t.get_id() );
        }
      )
    );
    DEBUG_LOG_BEGIN("ad_async_accept -2-" << t.get_id() );
    t.mutex().lock();
    DEBUG_LOG_END("ad_async_accept -1-" << t.get_id() );
  }
};

}}}}
