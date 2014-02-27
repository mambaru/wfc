#pragma once
#include <wfc/io/reader/errors/tags.hpp>
#include <wfc/io/reader/stream/async/tags.hpp>
#include <wfc/io/reader/common/tags.hpp>
#include <wfc/io/reader/tags.hpp>
#include <wfc/io/basic/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace io{ namespace reader{ namespace stream{ namespace async{

struct ad_async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ),
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
        { 
          
          //t.get_aspect().template get< common::_handler_>()(t, std::move(*dd), ec, bytes_transferred);
          t.get_aspect().template get< _outgoing_>()(t, std::move(*dd), ec, bytes_transferred);
        }
      )
    );
  }
};

}}}}}
