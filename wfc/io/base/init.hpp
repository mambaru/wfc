#pragma once

#include <boost/asio/io_service.hpp>

namespace wfc{ namespace io{ namespace base{

struct init
{
  boost::asio::io_service* io_service;
};

}}}
