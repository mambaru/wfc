#pragma once

#include <wfc/io/basic/tags.hpp>
#include <wfc/io/tags.hpp>

namespace wfc{ namespace io{ namespace basic{

struct ad_initialize
{
  template<typename T>
  void operator()(T& t, typename T::init_type& init)
  {
    typedef typename T::io_service_type io_service_type;
    std::shared_ptr<io_service_type> io_service = init.io_service;

    t.get_aspect().template get<_io_service_ptr_>() = io_service;

    typedef typename T::descriptor_type descriptor_type;
    t.get_aspect().template get<_descriptor_ptr_>() 
      = std::make_shared<descriptor_type>( *io_service );
      
    t.get_aspect().template get<_strand_>() 
      = std::make_shared<boost::asio::strand>( *io_service );
  }
};

}}}
