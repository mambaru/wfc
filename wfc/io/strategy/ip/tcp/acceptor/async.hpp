#pragma once

#include <wfc/io/context.hpp>
#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/acceptor/basic/aspect.hpp>
#include <wfc/io/acceptor/async/aspect.hpp>
#include <wfc/io/acceptor/loop/aspect.hpp>

namespace wfc{ namespace io{ namespace strategy{ namespace ip{ namespace tcp{ namespace acceptor{
  
struct async: fas::aspect<
  context<>,
  fas::type< ::wfc::io::_descriptor_type_, boost::asio::ip::tcp::acceptor >,
  fas::type< ::wfc::io::_data_type_, boost::asio::ip::tcp::socket >,
  fas::alias< wfc::io::acceptor::async::_outgoing_, wfc::io::acceptor::basic::_incoming_>,
  fas::alias< wfc::io::acceptor::basic::_outgoing_, wfc::io::acceptor::loop::_incoming_>,
  fas::alias< wfc::io::acceptor::loop::_accept_, wfc::io::acceptor::basic::_accept_>,
  fas::alias< wfc::io::acceptor::basic::_accept_some_, wfc::io::acceptor::async::_incoming_>,
  fas::alias< wfc::io::acceptor::loop::_outgoing_, wfc::io::acceptor::_outgoing_>,
  fas::alias< wfc::io::acceptor::_outgoing_, wfc::io::_incoming_>,
  ::wfc::io::acceptor::basic::aspect,
  ::wfc::io::acceptor::async::aspect,
  ::wfc::io::acceptor::loop::aspect,
  ::wfc::io::basic::aspect
  
>{};

}}}}}}
