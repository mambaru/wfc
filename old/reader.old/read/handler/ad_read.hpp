#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>

#include <wfc/io/reader/read/handler/tags.hpp>
#include <wfc/io/reader/common/tags.hpp>


namespace wfc{ namespace io{ namespace reader{ namespace read{ namespace handler{

struct ad_read
{
  template<typename T>
  struct return_type
  {
    typedef void type;
  };
  
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "handler::ad_read { " << std::endl;
    auto d = t.get_aspect().template get<common::_make_buffer_>()(t);
    t.get_aspect().template get< _outgoing_ >()(t, std::move(d));
    std::cout << "} handler::ad_read" << std::endl;
  }

};

}}}}}
