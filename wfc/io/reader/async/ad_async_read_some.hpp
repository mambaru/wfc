#pragma once
#include <boost/asio.hpp>
#include <wfc/io/reader/tags.hpp>
#include <wfc/io/reader/errors/tags.hpp>
namespace wfc{ namespace io{ namespace reader{ namespace async{

struct ad_async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "ad_async_read_some " << d->size() << std::endl;
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.get_aspect().template get<_descriptor_ptr_>()->async_read_some
    (
      ::boost::asio::buffer( **dd ), 
      [&t, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
      {
        std::cout << "ad_async_read_some ready" << std::endl;
        if (!ec)
        {
          std::cout << "ad_async_read_some bytes_transferred=" << bytes_transferred << std::endl;
          (*dd)->resize( bytes_transferred);
          t.get_aspect().template get<_ready_>()( t, std::move(*dd));
        }
        else if ( ec == boost::asio::error::operation_aborted)
        {
          t.get_aspect().template get< errors::_aborted_ >()(t, std::move(*dd) );
        }
        else
        {
          t.get_aspect().template get< errors::_error_ >()(t, ec, std::move(*dd) );
        }
      }
    );
  }
};

}}}}
