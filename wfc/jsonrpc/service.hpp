#pragma once

#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/handler/ihandler.hpp>
#include <wfc/jsonrpc/types.hpp>
#include <wfc/io/types.hpp>
#include <vector>
#include <string>

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

private:
  
  std::shared_ptr<service_impl> _impl;
  
};



}} 