#pragma once

#include <cstddef>
#include <memory>

namespace wfc{ namespace gateway{ 

  /*
template<typename Itf>    
struct iprovider
{
  typedef Itf interface_type;
  
  typedef std::shared_ptr<interface_type> interface_ptr;
  
  virtual ~iprovider() {}
  
  virtual void startup(size_t clinet_id, interface_ptr ptr ) = 0;

  virtual void shutdown(size_t clinet_id) = 0;

};

template<typename Itf>    
struct iprovider_impl: iprovider<Itf>
{
  typedef typename iprovider<Itf>::interface_ptr interface_ptr;
  typedef std::function<void(interface_ptr)> send_func;
  
  virtual void confirm() = 0;
  virtual void rollback() = 0;
  virtual void send(send_func fn) = 0;
};
*/

}}
