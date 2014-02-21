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
    std::cout << "ad_async_read_some " << std::endl;
    
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.descriptor().async_read_some
    (
      ::boost::asio::buffer( **dd ),
      t.get_aspect().template get< basic::_strand_>()->wrap(
        [this, &t, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
        { 
          std::cout << "ad_async_read_some ready" << std::endl;
          t.get_aspect().template get< common::_handler_>()(t, std::move(*dd), ec, bytes_transferred);
        }
      )
    );
  }
};

}}}}}
