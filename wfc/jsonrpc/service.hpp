#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/handler/ihandler.hpp>
#include <wfc/jsonrpc/types.hpp>
#include <wfc/io/types.hpp>
#include <vector>
#include <string>
#include <thread>
#include <list>

namespace wfc{ namespace jsonrpc{
  
class service_impl;

class service
{
public:
  typedef options options_type;
  typedef ihandler handler_interface;
  
  service( io_service& ios, const options_type& opt, const ihandler& jsonrpc_handler);
  
  void create_handler( io_id_t io_id,  outgoing_handler_t writer,  add_shutdown_handler_t add_shutdown );

  void operator()(  data_ptr d,  io_id_t id,  outgoing_handler_t callback);

  std::vector<std::string> get_methods() const;
  
  void start();

  void stop();
  
  std::shared_ptr<handler_interface> get_prototype() const;
  
  std::shared_ptr<handler_interface> clone_prototype() const;


private:
  typedef std::shared_ptr<service_impl> impl_ptr;
  // typedef std::shared_ptr<io_service> io_service_ptr;
  
  impl_ptr _impl;
  // int _threads;
  
  std::vector<impl_ptr> _impl_list;
  //std::vector<std::thread> _thread_list;
  //std::vector<io_service_ptr> _io_service_list;

  //void run_thread(int idx);
};



}} 