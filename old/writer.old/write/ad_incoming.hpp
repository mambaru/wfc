#pragma once
#include <boost/asio.hpp>
#include <wfc/memory.hpp>
#include <wfc/io/tags.hpp>

#include <wfc/io/writer/basic/tags.hpp>

namespace wfc{ namespace io{ namespace writer{ namespace write{

struct ad_incoming
{
  template<typename T>
  void operator()(T& t, std::size_t bytes_transferred)
  {
    // TODO: вернуть d в список буферов
    t.get_aspect().template get<_writed_size_>() += bytes_transferred;
  }

};

}}}}
