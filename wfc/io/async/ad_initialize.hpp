#pragma once

namespace wfc{ namespace io{ namespace async{

struct ad_initialize
{
  template<typename T>
  void operator()(T& t, typename T::init_type& init)
  {
    t.get_aspect().template get<_strand_>() 
      = std::make_shared<boost::asio::strand>( t.get_io_service() );
  }
};

}}}
