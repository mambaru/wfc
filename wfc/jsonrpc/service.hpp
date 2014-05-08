#pragma once

#include <wfc/io_service.hpp>
#include <wfc/io/types.hpp>
#include <wfc/jsonrpc/options.hpp>
#include <wfc/jsonrpc/handler/handler_base.hpp>
#include <vector>
#include <string>

namespace wfc{ namespace jsonrpc{
  
class service_impl;

class service
{
public:
  typedef options options_type;
  
  service( ::wfc::io_service& io_service, const options_type& opt, const handler_base& handler);
  
  void startup_handler( ::wfc::io::io_id_t io_id,  ::wfc::io::outgoing_handler_t writer,  ::wfc::io::add_shutdown_handler_t add_shutdown );

  void operator()(  ::wfc::io::data_ptr d,  ::wfc::io::io_id_t id,  ::wfc::io::outgoing_handler_t callback);

  std::vector<std::string> get_methods() const;
  
  void start();

  void stop();

private:
  
  std::shared_ptr<service_impl> _impl;
  
};



}} 