#pragma once

#include <boost/asio/io_service.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace basic{

struct init
{
  std::shared_ptr<boost::asio::io_service> io_service;
};

}}}
