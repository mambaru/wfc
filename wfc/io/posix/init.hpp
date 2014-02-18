#pragma once

#include <wfc/io/base/init.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace wfc{ namespace io{ namespace posix{

struct init: wfc::io::base::init
{
  boost::asio::posix::stream_descriptor::native_handle_type native_handle;
};

}}}
