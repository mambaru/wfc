#pragma once

#include <boost/asio.hpp>
#include <memory>

namespace wfc{ namespace io{




template<typename Tg>
struct async_read_some
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
          t.get_aspect().template get<Tg>()(t, std::move(*dd), ec, bytes_transferred);
        }
      )
    );
  }
};

}}
