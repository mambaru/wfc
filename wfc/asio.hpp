//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace wfc{ namespace asio{

typedef ::boost::asio::io_service io_service;
typedef ::boost::asio::strand strand;
typedef ::boost::asio::strand deadline_timer;

}}
