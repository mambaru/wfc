#pragma once 

#include <boost/asio.hpp>
#include <memory>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
  
typedef ::boost::asio::ip::tcp::acceptor acceptor_type;
typedef std::shared_ptr<acceptor_type> acceptor_ptr;

  
}}}}}
