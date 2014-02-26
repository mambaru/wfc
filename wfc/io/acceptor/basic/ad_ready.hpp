#pragma once

#include <wfc/io/acceptor/tags.hpp>
#include <wfc/io/acceptor/basic/tags.hpp>

namespace wfc{ namespace io{ namespace acceptor{ namespace basic{

struct ad_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "common ad_ready { " << std::endl;
    t.get_aspect().template gete<_on_accept_>()( t/*, *d*/ );
    t.get_aspect().template get<_outgoing_>()( t, std::move(d) );
    std::cout << "} common ad_ready " << std::endl;
  }
};

}}}}
