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
    
    t.descriptor().async_accept
    (
      **dd,
      t.strand().wrap(
        [this, &t, dd]( boost::system::error_code ec )
        { 
          
          if ( !t.descriptor().is_open() )
            return;
          
          
          t.get_aspect().template get< _outgoing_>()(t, std::move(*dd), ec);
        }
      )
    );
  }
};

}}}}
