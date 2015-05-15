#pragma once

#include <wfc/asio.hpp>
#include <wfc/service/rn/jsonrpc/service.hpp>

namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

class service_list
{
public:
  typedef ::wfc::asio::io_service io_service_type;
  service_list(std::weak_ptr< ::wfc::global > g, const service_list_config& conf);
  
  service_list(io_service_type& io, const service_list_config& conf, std::shared_ptr<ifactory> fact);
  
  void reconfigure(const service_list_config& conf);
  
  void initialize(std::shared_ptr<ifactory> fact);
  
  void start();
  
  void stop();
  
  void shutdown();
  
  static service_list_config create_config(std::string type);

private:
  void create( io_service_type& io, const service_list_config& conf, std::shared_ptr<ifactory> fact);

private:
  io_service_type& _io_service;
  service_list_config _conf;
  std::list< std::shared_ptr<service> > _service_list;
};

}}}}
