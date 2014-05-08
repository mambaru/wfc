#pragma once
#include <wfc/io/writer/stream/async/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace stream{ namespace async{

struct ad_async_write_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    t.descriptor().async_write_some
    (
      ::boost::asio::buffer( **dd ),
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
        { 
          t.get_aspect().template get< _outgoing_>()(t, std::move(*dd), ec, bytes_transferred);
        }
      )
    );
  }
};

}}}}}
