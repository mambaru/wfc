#pragma once
#include <wfc/io/reader/errors/tags.hpp>
#include <wfc/io/reader/tags.hpp>
#include <wfc/io/basic/tags.hpp>
#include <boost/asio.hpp>
namespace wfc{ namespace io{ namespace reader{ namespace async{

struct ad_async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.get_aspect().template get<_descriptor_ptr_>()->async_read_some
    (
      ::boost::asio::buffer( **dd ),
      t.get_aspect().template get< basic::_strand_>()->wrap(
      [&t, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
      {
        if (!ec)
        {
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
        )
    );
  }
};

}}}}
