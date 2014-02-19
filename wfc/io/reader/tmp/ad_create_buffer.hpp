#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace reader{

struct ad_create_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& )
  {
    return std::make_unique< typename T::data_type >( 8096 );
  }

};

}}}
