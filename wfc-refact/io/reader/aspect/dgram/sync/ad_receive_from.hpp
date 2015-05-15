#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <wfc/logger.hpp>
#include <wfc/io/reader/aspect/tags.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace dgram{ namespace sync{

struct ad_receive_from
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    typedef typename T::descriptor_type::endpoint_type endpoint_type;
    
    if ( !t.status() )
      return;
    
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    auto pep = std::make_shared<endpoint_type>();
    auto pthis = t.shared_from_this();
    
    auto callback = [pthis, dd, pep]( boost::system::error_code ec , std::size_t bytes_transferred )
    { 
      typename T::lock_guard lk(pthis->mutex());
      pthis->get_aspect().template get<_remote_endpoint_>() = std::move(*pep);
      pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(*dd), std::move(ec), bytes_transferred);
    };
    
    //t.mutex().unlock();
    t.descriptor().async_receive_from( ::boost::asio::buffer( **dd ), *pep, callback);
    //t.mutex().lock();

    /*
    if ( !t.status() )
      return;
    
    
    typedef typename T::descriptor_type::endpoint_type endpoint_type;
    
    endpoint_type ep;
    boost::system::error_code ec;
    
    t.mutex().unlock();
    std::size_t bytes_transferred =
      t.descriptor().receive_from( ::boost::asio::buffer(*d), ep, 0, ec);
    t.mutex().lock();
    
    t.get_aspect().template get<_remote_endpoint_>() = std::move(ep);
    t.get_aspect().template get<_read_handler_>()(t, std::move(d), std::move(ec), bytes_transferred);
    */
  }
};

}}}}}
