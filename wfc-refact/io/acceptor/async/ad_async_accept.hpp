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
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    auto pthis=t.shared_from_this();
    auto callback =         [pthis, dd]( boost::system::error_code ec )
        { 
          typename T::lock_guard lk( pthis->mutex() );
          
          if ( !pthis->status() )
            return;

          if ( !pthis->descriptor().is_open() )
            return;
          
          pthis->get_aspect().template get< _outgoing_>()(*pthis, std::move(*dd), ec);
        };
    //auto wcallback = t.owner().wrap(callback);

    
    t.mutex().unlock();
    t.descriptor().async_accept
    (
      **dd,
      callback
    );
    t.mutex().lock();
  }
};

}}}}
