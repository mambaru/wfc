#pragma once

#include <wfc/jsonrpc/service/basic_service.hpp>


namespace wfc{ namespace jsonrpc{
  

class service_impl
  : public basic_service< >
{
public:
  typedef basic_service<> super;

  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  service_impl(io_service_type& ios, const options_type& opt, const handler_base& jsonrpc_handler)
    : super( ios, opt, jsonrpc_handler)
  {
  }
};



}} 