#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <wfc/io/writer/aspect/dgram/tags.hpp>
#include <boost/system/error_code.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace writer{ namespace dgram{
  
struct ad_async_send_to
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    if ( !t.status() )
      return;
    
    if ( d == nullptr || d->empty() )
      return;

    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    auto pthis = t.shared_from_this();
    auto transfer_size = t.options().output_buffer_size;
    
    auto callback = [pthis, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
    { 
      typename T::lock_guard lk(pthis->mutex());
      pthis->get_aspect().template get<_write_handler_>()(*pthis, std::move(*dd), std::move(ec), bytes_transferred);
    };
    auto ep = t.get_aspect().template get<_remote_endpoint_>();
    //t.mutex().unlock();
    t.descriptor().async_send_to( ::boost::asio::buffer( **dd, transfer_size ), ep, callback);
    //t.mutex().lock();
  }
};

}}}}
