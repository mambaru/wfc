#pragma once

#include <wfc/service/rn/jsonrpc/service.hpp>

namespace wfc{ namespace service{ namespace rn{ namespace jsonrpc{

class service_list
{
public:
  service_list(std::weak_ptr<::wfc::global > g, const service_list_config& conf);
  
  service_list(::wfc::io_service& io, const service_list_config& conf, std::shared_ptr<ifactory> fact);
  
  void reconfigure(const service_list_config& conf);
  
  void initialize(std::shared_ptr<ifactory> fact);
  
  void start();
  
  void stop();

private:
  void create( ::wfc::io_service& io, const service_list_config& conf, std::shared_ptr<ifactory> fact);

private:
  ::wfc::io_service& _io_service;
  service_list_config _conf;
  std::list< std::shared_ptr<service> > _service_list;
};

}}}}
