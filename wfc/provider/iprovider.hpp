#pragma once

#include <wfc/provider/provider_config.hpp>

#include <cstddef>
#include <memory>
#include <list>



namespace wfc{ namespace provider{ 

template<typename Itf>  
class iprovider
{
public:
  typedef Itf interface_type;
  typedef std::shared_ptr<interface_type> interface_ptr;
  typedef std::function<void(size_t)> shudown_handler;
  typedef std::list<shudown_handler> shudown_handler_list;

  typedef std::function<void(size_t, interface_ptr )> startup_handler;
  typedef std::list<startup_handler> startup_handler_list;
  
  virtual ~iprovider(){}

  virtual void reconfigure(const provider_config& conf) = 0;
  
  virtual size_t ready_count() const  = 0;
  
  virtual interface_ptr get(size_t& client_id) const  = 0;
  
  virtual interface_ptr get() const  = 0;
  
  virtual void startup(size_t clinet_id, interface_ptr ptr )  = 0;

  virtual void shutdown(size_t clinet_id)  = 0;

  virtual void add_shudown_handler( shudown_handler sh )  = 0;
  
  virtual shudown_handler_list get_shudown_handlers( ) const  = 0;

  virtual void set_shudown_handlers( shudown_handler_list lst )  = 0;
};

}}
