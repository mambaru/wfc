#pragma once

namespace wfc{ namespace inet{
  
struct iconnection
{
  virtual ~iconnection() {}
  
  virtual void close() = 0;
  /*
  virtual void shutdown() = 0;
  */
};

}}
