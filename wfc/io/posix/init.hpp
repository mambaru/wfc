#pragma once

#include <wfc/io/basic/init.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace wfc{ namespace io{ namespace posix{

struct init: wfc::io::basic::init
{
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
};

}}}
