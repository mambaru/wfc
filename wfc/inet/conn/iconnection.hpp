#pragma once

#include <boost/asio.hpp>
#include <wfc/inet/types.hpp>

namespace wfc{ namespace inet{
  
struct iconnection
{
  virtual ~iconnection() {}
  
  virtual void close() = 0;
  
  virtual boost::asio::ip::address remote_address() = 0;
  
  virtual unsigned short remote_port() = 0;
  
  virtual void shutdown() = 0;
  
  virtual void on_read(data_ptr d) = 0;

  virtual void send(data_ptr d) = 0;  
};

}}
