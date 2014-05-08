#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace wfc{

typedef ::boost::asio::io_service io_service;
typedef ::boost::asio::strand strand;

}
