#pragma once

#include <boost/asio.hpp>

namespace wfc{ namespace inet{
  
struct iconnection
{
  virtual ~iconnection() {}
  
  virtual void close() = 0;
  
  virtual boost::asio::ip::address remote_address() = 0;
  
  virtual unsigned short remote_port() = 0;
  /*
  virtual void shutdown() = 0;
  */
};

}}
