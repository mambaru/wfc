#pragma once

#include <wfc/gateway/rn/jsonrpc/gateway.hpp>

namespace wfc{ namespace gateway{ namespace rn{ namespace jsonrpc{

class gateway_list
{
public:
  gateway_list(std::weak_ptr< ::wfc::global > g, const gateway_list_config& conf);
  
  gateway_list(::wfc::io_service& io, const gateway_list_config& conf, std::shared_ptr<ifactory> fact);
  
  void reconfigure(const gateway_list_config& conf);
  
  void initialize(std::shared_ptr<ifactory> fact);
  
  void start();
  
  void stop();
  
  static gateway_list_config create_config(std::string);

private:
  /* void create( ::wfc::io_service& io, const gateway_list_config& conf, std::shared_ptr<ifactory> fact); */

private:
  std::weak_ptr< ::wfc::global> _global;
  ::wfc::io_service& _io_service;
  gateway_list_config _conf;
  std::list< std::shared_ptr<gateway> > _gateway_list;
};

}}}}
